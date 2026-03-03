
#include "../include/video.h"
#include <string.h>

const uint8_t video_stream[] = {

    #include "video_data.inc"
};


unsigned long video_len = sizeof(video_stream);



void video_init(video_handler_t *video, const uint8_t *stream, unsigned long len)
{
    memset(video, 0, sizeof(*video));
    video->state.current_state = VIDEO_STATE_IDLE;
    video->state.previous_state = VIDEO_STATE_IDLE;
    video->stream = stream;
    video->cur = 0;
    video->len = len;

    video->frame_buf = NULL;
    video->frame_pixels = 0;
    video->tmp_delta = NULL;

    video->tx_line = NULL;
    video->pending_pixels = 0;
    video->frame_pos = 0;

    video->start_requested = false;
    video->tx_dma_busy = false;
    video->tx_started  = false;
}


bool video_set_frame_buffer(video_handler_t* video, uint8_t* frame_buf, const unsigned long framebuf_pixels) { 

    if (video == NULL || frame_buf == NULL || framebuf_pixels == 0) return false;
 

    // Validate with internally stored width*height 
    unsigned long required_size = video->config.height * video->config.width;
    if (framebuf_pixels < required_size) return false;

    video->frame_buf = frame_buf;
    video->frame_pixels = framebuf_pixels;
    return true;

}


// Attach delta buffer to video handle
bool video_set_delta_buffer(video_handler_t* video, uint8_t* delta) {

    if (video == NULL || delta == NULL) return false;
 
    // Validate with internally stored width*height 
    video->tmp_delta = delta;
    return true;
}


// Attach tx buffer to video handle
bool video_set_tx_buffer(video_handler_t* video, uint8_t* tx_buf, const unsigned long tx_len) { 

    if (video == NULL || tx_buf == NULL || tx_len == 0) return false;
 
    video->tx_line = tx_buf;
    //video->pending_pixels = tx_len;
    return true;
}


/// @brief Construct bgr565 palette from original should be called after parsing palette
/// @param video 
void video_build_lut(video_handler_t* video) { 
    for (int i = 0; i < MAX_PALETTE_COLORS; i++) {
            video->lut565[i] = pack_bgr565(video->palette[i]); // Would it be better to just transform the original palette?
    }
}



void rle_decode_frame(video_handler_t* video) { 

    unsigned long pixels_decoded = 0;
    unsigned long pos = video->cur;


    // Run length decode
    while (pixels_decoded < video->frame_pixels && pos < video->len) { 


        // Parse header byte
        uint8_t run_len = video->stream[pos] & 0x7F;
        bool is_run  = (video->stream[pos] & 0x80) != 0;
        pos++;

        if(is_run) { 

            uint8_t run_value = video->stream[pos];

            for (int i = 0; i < run_len; i++) { 
                video->tmp_delta[pixels_decoded] = run_value;
                pixels_decoded++;
            }

            pos++;
        }
        else { 

            for (int i = 0; i < run_len; i ++) { 
                video->tmp_delta[pixels_decoded] = video->stream[pos];
                pixels_decoded++;
                pos++;
            }
        }
    }

    // Update current stream pointer
    video->cur = pos; 
}


void delta_decode_frame(video_handler_t* video) { 

    unsigned long n = video->frame_pixels;

    for (int i = 0; i < n; i ++) { 
        video->frame_buf[i] = (uint8_t)(video->frame_buf[i] + video->tmp_delta[i]);
    }

}


// Tranmsit frame to LCD 
bool video_prepare_tx_line(video_handler_t* video) { 

    if (video->frame_pos >= video->frame_pixels) {
        return false; // nothing to send / invalid
    }

    const unsigned long remaining = video->frame_pixels - video->frame_pos;
    const unsigned long max_pixels = video->config.width * VIDEO_CHUNK_LINES;
    const unsigned long n_pixels= (remaining < max_pixels) ? remaining : max_pixels; // skeptical about how this is worded

    /* for (size_t j = 0; j < n; j++) { 
        const uint8_t idx = video->frame_buf[video->frame_pos + j];
        uint16_t pixel = pack_bgr565(video->palette[idx]);
        video->tx_line[2*j+0] = (uint8_t)(pixel >> 8);   // MSB first
        video->tx_line[2*j+1] = (uint8_t)(pixel & 0xFF); // LSB
    } */

    // Let's use a precomputed lookup table instead of performing manual lookups

    for (size_t i = 0; i < n_pixels; i++) { 
        const uint8_t idx = video->frame_buf[video->frame_pos + i];
        uint16_t pixel = video->lut565[idx]; 
        video->tx_line[2*i+0] = (uint8_t)(pixel >> 8);   // MSB first
        video->tx_line[2*i+1] = (uint8_t)(pixel & 0xFF); // LSB
    }


    video->pending_pixels = n_pixels;

    // Don't advance frame_pos yet, do after DMA completes
    return true;  
}


// probably make static inline
uint16_t pack_bgr565(const uint32_t c) {
    uint8_t r = (uint8_t)((c >> 16) & 0xFF);
    uint8_t g = (uint8_t)((c >>  8) & 0xFF);
    uint8_t b = (uint8_t)( c        & 0xFF);

    return (uint16_t)(((uint16_t)(b >> 3) << 11) |
                      ((uint16_t)(g >> 2) << 5)  |
                      ((uint16_t)(r >> 3) << 0));
}


parse_header_status_t parse_stream_header(video_handler_t* video) {



    if (video->len < VIDEO_STREAM_HEADER_SIZE) { 
        return HDR_INCOMPLETE; // the file can not be less than size of header
    }

    uint16_t file_format = (video->stream[0] << 8) | video->stream[1];
    uint16_t width       = (video->stream[2] << 8) | video->stream[3];
    uint16_t height      = (video->stream[4] << 8) | video->stream[5];
    uint16_t num_colors  = (video->stream[6] << 8) | video->stream[7];
    uint8_t codec_flags  = video->stream[8];

    if (file_format != FILE_ID) { 
        return HDR_ERR_INVALID_ID;
    }

    if (width == 0 || height == 0) { 
        return HDR_ERR_DIM_ZERO;
    };

    if (width > MAX_WIDTH || height > MAX_HEIGHT) { 
        return HDR_ERR_DIM_TOO_LARGE; 
    }

    if (num_colors == 0) { 
        return HDR_ERR_NO_COLORS;
    }

    if (num_colors > MAX_PALETTE_COLORS) { 
        return HDR_ERR_TOO_MANY_COLORS;
    }


    video->config.file_format = file_format;
    video->config.width       = width;
    video->config.height      = height; 
    video->config.num_colors  = num_colors;
    video->config.codec_flags = codec_flags;
    

    // Update file index 
    video->cur += VIDEO_STREAM_HEADER_SIZE; 

    return HDR_OK;
}


parse_palette_status_t parse_palette(video_handler_t* video) 
{
    unsigned long expected_palette_bytes;
    unsigned int palette_start;

    
    expected_palette_bytes = video->config.num_colors * PALETTE_BYTES_PER_COLOR;
    palette_start = VIDEO_STREAM_HEADER_SIZE; // Start after the header

    /* --- ensure palette fits in stream buffer --- */
    if (video->len < palette_start + expected_palette_bytes)
        return PAL_INCOMPLETE;

    /* --- parse palette --- */
    for (unsigned long i = 0; i < video->config.num_colors; i++) {
        unsigned int off = palette_start + (i * 3);

        uint8_t r = video->stream[off + 0];
        uint8_t g = video->stream[off + 1];
        uint8_t b = video->stream[off + 2];

        /* canonical internal format: 0x00RRGGBB */
        video->palette[i] = ((uint32_t)r << 16) |
                     ((uint32_t)g << 8)  |
                     ((uint32_t)b);
    }

    // Update current stream index
    video->cur += expected_palette_bytes;

    return PAL_OK;
}
