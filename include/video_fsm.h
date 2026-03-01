
#ifndef DECODER_FSM
#define DECODER_FSM


#ifdef __cplusplus 
extern "C" {
#endif


// 
//All logic here pertains to the state machine that carries out decoder and video playback states
//transitions 
//


#include "video.h" 


void video_sm_init(video_handler_t* video); 

void video_sm_run(video_handler_t* video);

void video_sm_transition(video_handler_t* video);


// Definition of state handlers 

void video_state_idle(video_handler_t* video);

void video_state_parse_header(video_handler_t* video);

void video_state_decode(video_handler_t* video);

void video_state_transmit(video_handler_t* video);



#ifdef __cplusplus 
}
#endif 


#endif // DECODER_FSM