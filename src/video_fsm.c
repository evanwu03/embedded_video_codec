

#include "../include/video_fsm.h"



typedef void (*state_handler_t)(video_handler_t *video);


static const state_handler_t state_handlers[] = { 
    [VIDEO_STATE_IDLE] = video_state_idle, 
    [VIDEO_STATE_PARSE_HEADER] = video_state_parse_header, 
    [VIDEO_STATE_DECODE_FRAME] = video_state_decode, 
    [VIDEO_STATE_TRANSMIT_FRAME] = video_state_transmit
};


void video_sm_init(video_handler_t* video) {




}

void video_sm_run(video_handler_t* video) { 




}


void video_sm_transition(video_handler_t* video) { 



}


// Definition of state handlers 

void video_state_idle(video_handler_t* video) { 



}


void video_state_parse_header(video_handler_t* video) { 



}

void video_state_decode(video_handler_t* video)  {



}


void video_state_transmit(video_handler_t* video) { 



}


