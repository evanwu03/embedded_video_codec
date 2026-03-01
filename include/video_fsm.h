
#ifndef DECODER_FSM
#define DECODER_FSM


// 
//All logic here pertains to the state machine that carries out decoder and video playback states
//transitions 
//


// Terminology
// sm - state machine
// ctx - context 


#include "video.h" 


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
     
    const uint8_t* stream;                // base pointer to video file 
    uint32_t palette[MAX_PALETTE_COLORS]; // pointer to palette 
    uint16_t palette_count;
    unsigned long cur;                     // current byte index into the stream
    unsigned long len;                     // total bytes in the stream

} video_handler_t;



void video_sm_init(video_handler_t* video); 

void video_sm_run(video_handler_t* video);

void video_sm_transition(video_handler_t* video);


// Definition of state handlers 

void video_state_idle(video_handler_t* video);

void video_state_parse_header(video_handler_t* video);

void video_state_decode(video_handler_t* video);

void video_state_transmit(video_handler_t* video);



#endif // DECODER_FSM