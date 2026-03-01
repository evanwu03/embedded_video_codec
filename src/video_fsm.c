

#include "../include/video_fsm.h"



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

    // Probably should clear flags here to

}



void video_sm_run(video_handler_t* video) { 
    // To-doDon't continue if video has errors maybe
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
    if(video->start_requested) { 
        video->start_requested = false;
        video_sm_transition(video, VIDEO_STATE_PARSE_HEADER);
    }

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

    // If successful start decoding
    video_sm_transition(video, VIDEO_STATE_DECODE_FRAME);

}

/// @brief VIDEO_STATE_DECODE Callback function
/// @param video 
void video_state_decode(video_handler_t* video)  {

    // begin RLE decoding frame
    rle_decode_frame(video);

    
    delta_decode_frame(video);

    video_sm_transition(video, VIDEO_STATE_TRANSMIT_FRAME);

}


/// @brief VIDEO_STATE_TRANSMIT Callback function
/// @param video 
void video_state_transmit(video_handler_t* video) { 

    unsigned long width = video->tx_line_pixels;

    if (video->frame_pos >= video->frame_pixels) { 
        // Done sending this frame
        video->frame_pos = 0; 
        video_sm_transition(video, VIDEO_STATE_DECODE_FRAME);
        return;
    }

    /*
    if DMA is not busy 

    video_prepare_tx_line(video);

    DMA is busy 
    send line to DMA 
    */
}


