// MASTER TX ADDED LED TO SEE TRANSFER

#include <stdint.h>
#include <stdbool.h>

#include <msp432p401r.h>


// HAL includes
#include "../hal/include/spi.h"
#include "../hal/include/uart.h"
#include "../hal/msp432/msp432_regs.h"
#include "../hal/include/wdt.h"
#include "../hal/include/gpio.h"

// LCD driver 
#include "../LcdDriver/lcd.h"


// Pixel art byte map
#include "../include/pixel_map.h"


// Video Player 
#include "../include/video.h"
#include "../include/video_fsm.h"


// User sets dimensions of LCD here
#define WIDTH 108
#define HEIGHT 122 

// Peripherals 
struct wdt wdt_a;
struct gpio led1;

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


static const struct wdt_config_t wdt_config_interval_timer_1s = {
    .mode_select = WDT_A_CTL_TMSEL, // Timer Interval Mode
    .interval_select = WDT_A_CTL_IS_4,
    .clock_source = WDT_A_CTL_SSEL_3,
    .counter_clear = WDT_A_CTL_CNTCL
};



// Video Handler 
video_handler_t video;
video_handler_t* video_ctx = &video;

// Frame buffers
static uint8_t frame_buf[WIDTH*HEIGHT];
static uint8_t tmp_delta[WIDTH*HEIGHT];
static uint8_t tx_buf[2*WIDTH];


int main(void)
{
    WDT_hold(&wdt_a);

    WDT_init(&wdt_a, WDT_A_BASE, &wdt_config_interval_timer_1s);
    NVIC_EnableIRQ(WDT_A_IRQn);

    UART_initModule(EUSCI_A0, &UART_A0_config); 
    UART_enableModule(EUSCI_A0); 

    // Enable UART0 Pins
    // P1.2->RX
    // P1.3->TX
    P1->SEL0 |= BIT2 | BIT3;
    P1->SEL1 &= ~(BIT2 | BIT3);

    
    gpio_init_output(&led1, PORT1_BASE, BIT0);
    gpio_write(&led1, false); // Turn off LED initially


    lcd_init();
    lcd_dma_init();


    // Video configurations
    video_init(&video, video_stream, video_len);
    video_set_frame_buffer(&video, frame_buf, WIDTH*HEIGHT);
    video_set_delta_buffer(&video, tmp_delta);
    video_set_tx_buffer(&video, tx_buf, WIDTH);

    
    // Initialize state machine
    video_sm_init(&video);



    __enable_irq();

    //lcd_draw_image(wolf_girl_map, 0, 0, 128, 128);

    
    // Request start 
    video.start_requested = true;

    // Run state machine here
    while (1)
    {
        video_sm_run(&video);
    }
   
}


void WDT_A_IRQHandler(void) {
       gpio_toggle(&led1);
}


