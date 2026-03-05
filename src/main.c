// MASTER TX ADDED LED TO SEE TRANSFER

#include <stdint.h>
#include <stdbool.h>
#include <assert.h>

#include <msp432p401r.h>


// HAL includes
#include "../hal/include/uart.h"
#include "../hal/msp432/msp432_regs.h"
#include "../hal/include/gpio.h"

// Driverlib
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>


// LCD driver 
#include "../LcdDriver/lcd.h"


// Pixel art byte map
//#include "../include/pixel_map.h"


// Video Player 
#include "../include/video.h"
#include "../include/video_fsm.h"


// User sets dimensions of LCD here
#define WIDTH 128
#define HEIGHT 128

// Peripherals 
// struct gpio led1;

// Peripheral Configurations 
static const UART_config_t UART_A0_config = {
    .parity = UART_PARITY_NONE, 
    .order  = UART_LSB_FIRST, 
    .data_length = UART_DATA_8BIT, 
    .mode = UART_MODE, 
    .clock_sel = UART_SMCLK, 
    .baud_rate = 9600, 
    .oversampling = UART_OVERSAMPLING_ON,
    .baud_prescaler = 19,
    .firstMod  = 9,
    .secondMod = 0xAA
};


// Video Handler 
video_handler_t video;
video_handler_t* video_ctx = &video;

// Frame buffers
static uint8_t frame_buf[WIDTH*HEIGHT];
static uint8_t tmp_delta[WIDTH*HEIGHT];

// NOTE: Apparently DMA (PL230) is limited to 1024 data transfers in otherwards buffer is limited to 1024 bytes on 8-bit SPI
static uint8_t tx_buf[2*WIDTH*VIDEO_CHUNK_LINES];
_Static_assert(sizeof(tx_buf)/sizeof(uint8_t) <= 1024, "DMA in MSP432 is limited up to 1024 data transfers");




int main(void)
{

    // Clock configuration stuff
    CS_setDCOCenteredFrequency(CS_DCO_FREQUENCY_24);
    CS_initClockSignal(CS_SMCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_2);


    // UART configuration
    uart_initModule(EUSCI_A0, &UART_A0_config); 
    uart_enableModule(EUSCI_A0); 

    // Enable UART0 Pins
    // P1.2->RX
    // P1.3->TX
    P1->SEL0 |= BIT2 | BIT3;
    P1->SEL1 &= ~(BIT2 | BIT3);

    

    // Initalize LCD screen
    lcd_init();
    lcd_dma_init();


    // Initialize video handler
    video_init(&video, video_stream, video_len);
    video_set_frame_buffer(&video, frame_buf);
    video_set_delta_buffer(&video, tmp_delta);
    video_set_tx_buffer(&video, tx_buf, 2*WIDTH*VIDEO_CHUNK_LINES);
    
    // Initialize state machine
    video_sm_init(&video);


    __enable_irq();

    //lcd_draw_image(wolf_girl_map, 0, 0, 128, 128);

    
    // Request start 
    // To-do user can press button to start video
    video.start_requested = true; // Doesn't do anything right now

    // Run state machine here
    while (1)
    {
        video_sm_run(&video);
    }
   
}



