#ifndef CHIP8_DISPLAY_H
#define CHIP8_DISPLAY_H

#include <stdint.h>
#include "../common.h"


/**
 *  64x64 pixel
 *  pixel = 1 bit
 *  buffer:
 *   width  = 64px = 64 bit = 8 byte
 *   height = 64px = 64 bit = 8 byte
 *
 *  pixel = 1 = white
 *  pixel = 0 = black
 */
typedef struct {
    uint8_t buffer[DISPLAY_BUFFER_SIZE];
    uint8_t draw_flag;
} Display;

Display * create_display();

void init_display(Display *display);

/**
 *
 * @param display
 * @param row_pixel
 * @param col_pixel
 * @param sprite n bytes array the represent the sprite every byte in array if row_pixel
 * @param sprite_size size of sprite in bytes
 * @return 1 if any pixel turned off because writing the sprite
 */
uint8_t display_sprite(Display * display, size_t row_pixel, size_t col_pixel, const uint8_t * sprite, uint8_t sprite_size);

void clear_display(Display *display);

void free_display(Display * display);

#endif //CHIP8_DISPLAY_H
