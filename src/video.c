
#include "../include/video.h"
#include "../include/video_fsm.h"


const uint8_t video_stream[] = {

    #include "video_data.inc"
};


unsigned long video_len = sizeof(video_stream);


parse_header_status_t parse_stream_header(video_stream_header_t* hdr, const uint8_t stream[], const unsigned long file_len) {

    if (file_len < VIDEO_STREAM_HEADER_SIZE) { 
        return HDR_INCOMPLETE; // the file can not be less than size of header
    }

    uint16_t file_format = (stream[0] << 8) | stream[1];
    uint16_t width       = (stream[2] << 8) | stream[3];
    uint16_t height      = (stream[4] << 8) | stream[5];
    uint16_t num_colors  = (stream[6] << 8) | stream[7];
    uint8_t codec_flags  = stream[8];

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


    hdr->file_format = file_format;
    hdr->width       = width;
    hdr->height      = height; 
    hdr->num_colors  = num_colors;
    hdr->codec_flags = codec_flags;

    return HDR_OK;
}


parse_palette_status_t parse_palette(
    uint32_t *palette,
    const uint8_t *stream,
    unsigned long stream_len,
    unsigned long num_colors
)
{
    unsigned long expected_palette_bytes;
    unsigned int palette_start;

    
    expected_palette_bytes = num_colors * PALETTE_BYTES_PER_COLOR;
    palette_start = VIDEO_STREAM_HEADER_SIZE; // Start after the header

    /* --- ensure palette fits in stream buffer --- */
    if (stream_len < palette_start + expected_palette_bytes)
        return PAL_INCOMPLETE;

    /* --- parse palette --- */
    for (unsigned long i = 0; i < num_colors; i++) {
        unsigned int off = palette_start + (i * 3);

        uint8_t r = stream[off + 0];
        uint8_t g = stream[off + 1];
        uint8_t b = stream[off + 2];

        /* canonical internal format: 0x00RRGGBB */
        palette[i] = ((uint32_t)r << 16) |
                     ((uint32_t)g << 8)  |
                     ((uint32_t)b);
    }

    return PAL_OK;
}
