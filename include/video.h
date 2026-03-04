

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

// User defines this depending on resolution of display 
#define MAX_WIDTH (128U)  
#define MAX_HEIGHT (128U)

// This file ID is expected at very beginning of header
#define FILE_ID (0x5643U)

// Maximum palette colors and Bytes per pixel (bpp)
#define MAX_PALETTE_COLORS (256U)
#define PALETTE_BYTES_PER_COLOR (3U)

// Size of stream header in bytes
#define VIDEO_STREAM_HEADER_SIZE 9U 

// Number of lines in DMA buffer
#define VIDEO_CHUNK_LINES 4U 

/// @brief defines the stream header containing metadata for a video file
typedef struct { 
    uint16_t file_format;
    uint16_t width;
    uint16_t height;
    uint16_t num_colors;
    uint8_t codec_flags;
} video_stream_header_t;


#ifdef __cplusplus 
static_assert(sizeof(video_stream_header_t) >= VIDEO_STREAM_HEADER_SIZE, "video_stream_header_t can not hold parsed field. Please check implementation");
#else 
_Static_assert(sizeof(video_stream_header_t) >= VIDEO_STREAM_HEADER_SIZE, "video_stream_header_t can not hold parsed field. Please check implementation");
#endif




// Terminology
// sm - state machine
// ctx - context 

typedef enum { 
    VIDEO_STATE_IDLE = 0,
    VIDEO_STATE_PARSE_HEADER,
    VIDEO_STATE_DECODE_FRAME,
    VIDEO_STATE_TRANSMIT_FRAME
} video_state_t;


typedef struct { 
    video_state_t current_state;
    video_state_t previous_state;

    // To-do:  Error flags can be included here if any are implemented  in future

} video_sm_ctx_t;


typedef struct { 
    video_stream_header_t config;
    video_sm_ctx_t state; 
     
    const uint8_t* stream;                 // base pointer to video file 
    uint32_t palette[MAX_PALETTE_COLORS];  // pointer to palette 
    uint16_t lut565[MAX_PALETTE_COLORS];
    unsigned long cur;                     // current byte index into the stream
    unsigned long len;                     // total bytes in the stream


    uint8_t* frame_buf;                    // pointer to current frame indices
    unsigned long frame_pixels;            // size of framebuffer in pixels 
    uint8_t* tmp_delta;                    // pointer to hold frames delta  
    uint8_t* tx_line;                     // pointer to 128-pixel line buffer, assumes 16-bit BGR values for now

    unsigned long pending_pixels;          // size of tx_line in pixels
    unsigned long frame_pos;               // current 

    bool start_requested;
    bool tx_started;
    volatile bool tx_dma_busy;

} video_handler_t;



void video_init(video_handler_t* video, const uint8_t *stream, unsigned long len);

// Attaches frame buffer to video handle
bool video_set_frame_buffer(video_handler_t* video, uint8_t* framebuf, const unsigned long framebuf_len);

// Attach delta buffer to video handle
bool video_set_delta_buffer(video_handler_t* video, uint8_t* delta);

// Attach tx buffer to video handle
bool video_set_tx_buffer(video_handler_t* video, uint8_t* tx_buf, const unsigned long tx_len);

// Computes lookup table from palette
void video_build_lut(video_handler_t* video); 

// Decoding Functions
void rle_decode_frame(video_handler_t* video); 

void delta_decode_frame(video_handler_t* video);

// Prepare tx line buffer for sending to LCD
bool video_prepare_tx_line(video_handler_t* video);


uint16_t pack_bgr565(const uint32_t bgr32);


typedef enum {
    HDR_OK,
    HDR_INCOMPLETE,
    HDR_ERR_INVALID_ID,
    HDR_ERR_DIM_ZERO,
    HDR_ERR_DIM_TOO_LARGE,
    HDR_ERR_NO_COLORS,
    HDR_ERR_TOO_MANY_COLORS
} parse_header_status_t;


typedef enum { 
    PAL_OK,
    PAL_INCOMPLETE,
} parse_palette_status_t;


// Parsing Functions
parse_header_status_t parse_stream_header(video_handler_t* video);

parse_palette_status_t parse_palette(video_handler_t* video);




#ifdef __cplusplus 
}
#endif 

#endif