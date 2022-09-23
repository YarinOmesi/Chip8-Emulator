#include "display.h"
#include <stdlib.h>
#include <string.h>
#include <memory.h>

Display *create_display() {
    return (Display *) malloc(sizeof(Display));
}

void init_display(Display *display) {
    clear_display(display);
    display->draw_flag = 1;
}

void clear_display(Display *display) {
    memset(display->buffer, 0, DISPLAY_BUFFER_SIZE);
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
 * @param x x in pixels
 * @param y y in pixels
 * @param sprite
 * @param sprite_size in bytes
 * @return
 */
uint8_t display_sprite(Display *display, size_t x, size_t y, const uint8_t *sprite, uint8_t sprite_size) {
    uint8_t row = y;
    uint8_t col = x / 8;
    uint8_t index_in_cell = x % 8;

    uint8_t is_pixel_turned_off = 0;

    for (int sprite_row = 0; sprite_row < sprite_size; ++sprite_row) {

        uint8_t cells_values[] = {
                (sprite[sprite_row] >> index_in_cell),                            // cell at (row, col)
                sprite[sprite_row] << (CHIP8_DISPLAY_WIDTH_BYTES - index_in_cell) // cell at (row, col + 1)
        };
        for (int i = 0; i < 2; ++i) {
            uint8_t cell_value = cells_values[i];
            if (cell_value != 0) {
                uint8_t *buffer = buffer_at(display, row + sprite_row, col + i);
                (*buffer) ^= cell_value;
                is_pixel_turned_off |= (*buffer) == cell_value ? 0 : 1;
            }
        }
    }
    display->draw_flag = 1;
    return is_pixel_turned_off;
}