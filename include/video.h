

#ifndef VIDEO_H
#define VIDEO_H

#ifdef __cplusplus 
extern "C" {
#endif

#include <stdint.h>
#include <assert.h>
#include <stdbool.h>

extern const uint8_t video_stream[];

extern unsigned long video_len;


/// @brief defines the stream header containing metadata for a video file
typedef struct { 
    uint16_t file_format;
    uint16_t width;
    uint16_t height;
    uint16_t num_colors;
    uint8_t codec_flags;
} video_stream_header_t;

#define VIDEO_STREAM_HEADER_SIZE 9U 
_Static_assert(sizeof(video_stream_header_t) >= VIDEO_STREAM_HEADER_SIZE, "video_stream_header_t can not hold parsed field. Please check implementation");


// User defines this depending on resolution of display 
#define MAX_WIDTH (128U)  
#define MAX_HEIGHT (128U)

// This file ID is expected at very beginning of header
#define FILE_ID (0x5643U)


typedef enum {
    HDR_OK,
    HDR_INCOMPLETE,
    HDR_ERR_INVALID_ID,
    HDR_ERR_DIM_ZERO,
    HDR_ERR_DIM_TOO_LARGE,
    HDR_ERR_NO_COLORS,
    HDR_ERR_TOO_MANY_COLORS
} parse_header_status_t;



// Maximum palette colors and Bytes per pixel (bpp)
#define MAX_PALETTE_COLORS (256U)
#define PALETTE_BYTES_PER_COLOR (3U)

typedef enum { 
    PAL_OK,
    PAL_INCOMPLETE,
} parse_palette_status_t;


// Functions
parse_header_status_t parse_stream_header(
    video_stream_header_t* hdr, 
    const uint8_t stream[],
    const unsigned long file_len
);

parse_palette_status_t parse_palette(
    uint32_t *palette,
    const uint8_t *buf,
    unsigned long buf_len,
    unsigned long num_colors
);




#ifdef __cplusplus 
}
#endif 

#endif