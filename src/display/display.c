#include "display.h"
#include <stdlib.h>
#include <string.h>

Display *create_display() {
    return (Display *) malloc(sizeof(Display));
}

void init_display(Display *display) {
    clear_display(display);
    display->draw_flag = 1;
}

void clear_display(Display *display) {
    for (int row = 0; row < DISPLAY_BUFFER_SIZE; ++row) {
        display->buffer[row] = 0;
    }
    display->draw_flag = 1;
}

void free_display(Display *display) {
    free(display);
}

static uint8_t *buffer_at(Display *display, uint8_t row, uint8_t col) {
    size_t index = (row * CHIP8_DISPLAY_WIDTH_BYTES + col);
    return display->buffer + index;
}

/**
 *
 * @param display
 * @param row_pixel row_pixel in pixels
 * @param col_pixel col_pixel in pixels
 * @param sprite
 * @param sprite_size in bytes
 * @return
 */
uint8_t display_sprite(Display *display, size_t row_pixel, size_t col_pixel, const uint8_t *sprite, uint8_t sprite_size) {
    uint8_t row = row_pixel;
    uint8_t col = col_pixel / SPRITE_SIZE_IN_BYTES;
    uint8_t index_in_cell = col_pixel - (col * SPRITE_SIZE_IN_BYTES);

    uint8_t is_pixel_turned_off = 0;
    for (int sprite_row = 0; sprite_row < sprite_size; ++sprite_row) {

        if (index_in_cell == 0) {
            // sprite is in only one cell
            uint8_t *buffer = buffer_at(display, row + sprite_row, col);
            (*buffer) ^= sprite[sprite_row];
            is_pixel_turned_off |= (*buffer) == sprite[sprite_row] ? 0 : 1;
        } else {
            // sprite is on two cells
            uint8_t pixels_in_first_cell = (sprite[sprite_row] >> index_in_cell);
            uint8_t *first_pixel_ptr = buffer_at(display, row + sprite_row, col);
            (*first_pixel_ptr) ^= pixels_in_first_cell;
            is_pixel_turned_off |= (*first_pixel_ptr) == pixels_in_first_cell ? 0 : 1;

            uint8_t pixels_in_second_cell = sprite[sprite_row] << (CHIP8_DISPLAY_WIDTH_BYTES - index_in_cell);
            // TODO: think about overflow
            uint8_t *second_pixel_ptr = buffer_at(display, row, col+1);
            (*second_pixel_ptr) = pixels_in_second_cell;
            is_pixel_turned_off |= (*second_pixel_ptr) == pixels_in_second_cell ? 0 : 1;
        }
    }
    display->draw_flag = 1;
    return is_pixel_turned_off;
}