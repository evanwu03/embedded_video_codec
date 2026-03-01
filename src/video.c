
#include "../include/video.h"
#include <string.h>

const uint8_t video_stream[] = {

    #include "video_data.inc"
};


unsigned long video_len = sizeof(video_stream);



void video_init(video_handler_t *v, const uint8_t *stream, unsigned long len)
{
    memset(v, 0, sizeof(*v));
    v->state.current_state = VIDEO_STATE_IDLE;
    v->state.previous_state = VIDEO_STATE_IDLE;
    v->stream = stream;
    v->cur = 0;
    v->len = len;
    
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

    return PAL_OK;
}
