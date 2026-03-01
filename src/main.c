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


#define WIDTH 128
#define HEIGHT 128 
 

// current frame palette index buffer
static uint8_t frame_buf[WIDTH*HEIGHT];


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
    


    __enable_irq();


    //lcd_draw_image(shark_square_128x128_map, 0, 0, WIDTH, HEIGHT);
    //lcd_draw_image(bocchi_twin_map, 0, 0, WIDTH, HEIGHT);
    //lcd_draw_image(kokomi_128x128_map, 0, 0, WIDTH, HEIGHT);
    //lcd_draw_image(ronald_wojak_128x128_map, 0, 0 , WIDTH, HEIGHT);
    //lcd_draw_image(hatsune_miku_128x128_map, 0, 0,  WIDTH, HEIGHT);
    //lcd_draw_image(orca_128x128_map, 0, 0, WIDTH, HEIGHT);
    lcd_draw_image(wolf_girl_map, 0, 0, WIDTH, HEIGHT);

    
    while (1)
    {

    }
   
}


void WDT_A_IRQHandler(void) {
       gpio_toggle(&led1);
}


