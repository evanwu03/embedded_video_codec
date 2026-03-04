

#include "hal_lcd.h"

#include <msp432p401r.h>
#include <core_cm4.h>

#include "../hal/msp432/msp432_regs.h"
#include "../hal/include/spi.h"
#include "../hal/include/gpio.h"


// LCD pin definitions
#define LCD_DC_PORT         PORT3_BASE  // P3.7
#define LCD_DC_PIN          BIT7
#define LCD_CS_PORT         PORT5_BASE  // P5.0
#define LCD_CS_PIN          BIT0
#define LCD_SPI_CLK_PORT    PORT1_BASE  // P1.5
#define LCD_SPI_CLK_PIN     BIT5
#define LCD_SPI_MOSI_PORT   PORT1_BASE  // P1.6
#define LCD_SPI_MOSI_PIN    BIT6
#define LCD_SPI_MISO_PORT   PORT1_BASE  // P1.7
#define LCD_SPI_MISO_PIN    BIT7
#define LCD_RST_PORT        PORT5_BASE  // P5.7
#define LCD_RST_PIN         BIT7



// LCD pins
struct gpio lcd_dc;
struct gpio lcd_cs;
struct gpio lcd_spi_mosi;
//struct gpio lcd_spi_miso;
struct gpio lcd_spi_clk;
struct gpio lcd_rst;


// Check what parameters the ST7735 Requires
/* Parameters to Configure eUSCI B0 Module for SPI mode */
    static const SPI_Config_t spi_config = { 
    .clock_sel = SPI_SMCLK,
    .mode = SPI_MASTER_MODE, 
    .data_order = SPI_MSB_FIRST, 
    .data_length = SPI_DATA_8BIT,
    .clock_phase_sel = SPI_PHASE_DATA_CAPTURED_ONFIRST_CHANGED_ON_NEXT, 
    .clock_polarity_sel = SPI_INACTIVE_LOW,  
    .clock_divider = 1 
};



void HAL_LCD_PORT_init(void) { 

    gpio_init_output(&lcd_spi_mosi, LCD_SPI_MOSI_PORT, LCD_SPI_MOSI_PIN);
    gpio_set_function(&lcd_spi_mosi, GPIO_AFSEL1);


    /* gpio_init_output(&lcd_spi_miso, LCD_SPI_MISO_PORT, LCD_SPI_MISO_PIN);
    gpio_set_function(&lcd_spi_miso, GPIO_AFSEL1); */

    gpio_init_output(&lcd_spi_clk, LCD_SPI_CLK_PORT, LCD_SPI_CLK_PIN);
    gpio_set_function(&lcd_spi_clk, GPIO_AFSEL1);

    gpio_init_output(&lcd_dc, LCD_DC_PORT, LCD_DC_PIN);
    gpio_init_output(&lcd_cs, LCD_CS_PORT, LCD_CS_PIN);

    gpio_init_output(&lcd_rst, LCD_RST_PORT, LCD_RST_PIN);

}


void HAL_LCD_SPI_init(void) { 

    // Initialize and enable SPI module 
    spi_initModule(EUSCI_B0, &spi_config);
    spi_enableModule(EUSCI_B0);

    gpio_write(&lcd_cs, true);
    gpio_write(&lcd_dc, true); // Set to data mode by default
}



void HAL_LCD_write_command(uint8_t command) { 

    gpio_write(&lcd_dc, false);
    gpio_write(&lcd_cs, false);

    spi_sendByte(EUSCI_B0, command);

    //gpio_write(&lcd_cs, true);
} 



void HAL_LCD_write_data(uint8_t data) { 

    gpio_write(&lcd_dc, true);
    gpio_write(&lcd_cs, false);

    spi_sendByte(EUSCI_B0, data);
    
    //gpio_write(&lcd_cs, true);
}


void HAL_LCD_delay(volatile uint32_t cycles) { 

    while(cycles--)
        __NOP();
}