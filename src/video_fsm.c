

#include "../include/video_fsm.h"
#include "../LcdDriver/lcd.h"
#include "../LcdDriver/hal_lcd.h"
#include "../hal/include/gpio.h"

#include <string.h> 


typedef void (*state_handler_t)(video_handler_t *video);


static const state_handler_t state_handlers[] = { 
    [VIDEO_STATE_IDLE] = video_state_idle, 
    [VIDEO_STATE_PARSE_HEADER] = video_state_parse_header, 
    [VIDEO_STATE_DECODE_FRAME] = video_state_decode, 
    [VIDEO_STATE_TRANSMIT_FRAME] = video_state_transmit
};


/// @brief Initializes the state machine
/// @param video 
void video_sm_init(video_handler_t* video) {

    video->state.previous_state = VIDEO_STATE_IDLE;
    video->state.current_state  = VIDEO_STATE_IDLE;
}



void video_sm_run(video_handler_t* video) { 
    // To-do Don't continue if video has errors maybe
    state_handlers[video->state.current_state](video);
}


void video_sm_transition(video_handler_t* video, video_state_t next_state){ 
    video->state.previous_state = video->state.current_state;
    video->state.current_state  = next_state;

    // to-do: maybe we want the current tick?

}


// Definition of state handlers 


/// @brief VIDEO_STATE_IDLE Callback function
/// @param video 
void video_state_idle(video_handler_t* video) { 

    // Start of video has been requested
    /* if(video->start_requested) { 
        video->start_requested = false;
        video->cur = 0; // Make sure we start reading from start of file
        video_sm_transition(video, VIDEO_STATE_PARSE_HEADER);
    } */
    video->cur = 0; // Make sure we start reading from start of file
    video_sm_transition(video, VIDEO_STATE_PARSE_HEADER);

    // Do nothing otherwise
}

/// @brief VIDEO_STATE_PARSE_HEADER Callback function
/// @param video 
void video_state_parse_header(video_handler_t* video) { 


    // Call parsing functions
    parse_header_status_t header_status = parse_stream_header(video);

    if(header_status != HDR_OK) { 
        video_sm_transition(video, VIDEO_STATE_IDLE); // Maybe we should be an error state
        return;
    }

    parse_palette_status_t palette_status = parse_palette(video);


    if(palette_status != PAL_OK) { 
        video_sm_transition(video, VIDEO_STATE_IDLE); // Maybe we should go to an error state
        return;
    }

    // Build lookup table for bgr565 or whichever color format
    video_build_lut(video);

    // What happens if i just set lcd address window once?
    lcd_set_window(0, 0, video->config.width-1, video->config.height-1);

    // If successful transition to decoding state
    video_sm_transition(video, VIDEO_STATE_DECODE_FRAME);

}

/// @brief VIDEO_STATE_DECODE Callback function
/// @param video 
void video_state_decode(video_handler_t* video)  {

    // Done streaming video
    if (video->cur >= video->len) { 
        //video_sm_transition(video, VIDEO_STATE_IDLE);
        video->cur = VIDEO_STREAM_HEADER_SIZE + video->config.num_colors * PALETTE_BYTES_PER_COLOR;
        
        // reset frame and delta buffers 
        // To-do: see if there is a more optimal way to clear the buffer, for now though this is fine for small resolutions
        memset(video->tmp_delta, 0, video->frame_pixels);
        memset(video->frame_buf, 0, video->frame_pixels);
        return;
    }


    // To-do should decode functions report status? 

    // begin RLE decoding frame
    rle_decode_frame(video);

    // decode deltas frames
    delta_decode_frame(video);

    video_sm_transition(video, VIDEO_STATE_TRANSMIT_FRAME);
}


/// @brief VIDEO_STATE_TRANSMIT Callback function
/// @param video 
void video_state_transmit(video_handler_t* video) { 

    // finished?
    if (video->frame_pos >= video->frame_pixels) {
        // Must pull CS back high after writing
        gpio_write(&lcd_cs, true);
        gpio_write(&lcd_dc, false);
        video->frame_pos = 0;
        video->tx_started = false;
        video_sm_transition(video, VIDEO_STATE_DECODE_FRAME);
        return;
    }

    // If a DMA transfer is in progress, do nothing this tick.
    // To-do we don't necessarily want the CPU to do nothing. We should be double buffering DMA
    if (video->tx_dma_busy) {
        return;
    }

    // One-time per frame: set LCD window and start RAM write stream
    if (!video->tx_started) {
        lcd_ramwr(); 
        video->tx_started = true;
        // fall through and send first line immediately (or return if you prefer)
    }

    // Prepare next line of pixels into tx_line
    video_prepare_tx_line(video);

    // Start DMA on this line (width * 2 bytes)
    lcd_tx_pixels_dma(video->tx_line, 2*video->pending_pixels); //  To-Do don't make this hardcoded

    // Advance frame_pos AFTER DMA completes (recommended)
    // So do NOT increment here; increment in DMA done callback.
}



