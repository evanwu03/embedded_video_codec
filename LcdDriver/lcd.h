
#ifndef LCD_H
#define LCD_H


#include <stdint.h>
#include <stdbool.h>


/// @brief Initializes LCD display 
void lcd_init();


/// @brief Sets frame address pointer that will be written to
/// @param x0 start x coordinate
/// @param y0 start y coordinate
/// @param x1 end x coordinate
/// @param y1 end y coordinate
void lcd_set_window(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1); // Wrap coordinates into struct in future?



/// @brief Initializes DMA SPI channel 
void lcd_dma_init();


/// @brief send pixel buffer over SPI DMA
/// @param buf pointer to buffef 
/// @param width size of buffer
bool lcd_tx_pixels_dma(uint8_t* buf, unsigned long bytes);



/// @brief Enables memory write mode for LCD 
void lcd_ramwr(); 

/// @brief Draw a single pixel at (x,y) pixel
/// @param x x coordinate
/// @param y y coordinate
/// @param red   red value 
/// @param green green value
/// @param blue  blue value
void lcd_draw_pixel(const uint16_t x, const uint16_t y, const uint16_t red, const uint16_t green, const uint16_t blue); // Wrap RGB into struct?



/// @brief Clears the screen and fills it with black
void lcd_clear_screen();


void lcd_draw_image(const uint8_t* pixels, uint8_t startX, uint8_t startY, uint8_t width, uint8_t height);

#endif // LCD_H
