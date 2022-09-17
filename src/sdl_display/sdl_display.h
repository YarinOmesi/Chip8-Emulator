#ifndef CHIP8_SDL_DISPLAY_H
#define CHIP8_SDL_DISPLAY_H

#define SDL_MAIN_HANDLED
#include "SDL.h"
#include "../common.h"

typedef struct {
    SDL_Window * window;
    SDL_Renderer *renderer;
} SDLDisplay;

uint8_t init_sdl_display(SDLDisplay * sdlDisplay);

void render(SDLDisplay * sdlDisplay, const uint8_t * displayBuffer, size_t size);

void free_sdl_display(SDLDisplay * sdlDisplay);

#endif //CHIP8_SDL_DISPLAY_H
