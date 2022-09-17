#include <stdio.h>
#include "sdl_display.h"

uint8_t init_sdl_display(SDLDisplay *sdlDisplay) {
    SDL_Window *window = NULL;
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Video init error %s\n", SDL_GetError());
        return 0;
    }

    window = SDL_CreateWindow(WINDOW_NAME, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                              WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);

    if (!window) {
        printf("Could not create window");
        return 0;
    }
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    if (!renderer) {
        printf("Could not create renderer");
        return 0;
    }

    sdlDisplay->window = window;
    sdlDisplay->renderer = renderer;

    SDL_RenderSetLogicalSize(renderer, CHIP8_DISPLAY_WIDTH, CHIP8_DISPLAY_HEIGHT);
    SDL_RenderSetIntegerScale(renderer, 1);
    return 1;
}


static void render_display_to_surface(SDL_Surface *surface,const uint8_t *displayBuffer, size_t size) {
    for (int i = 0; i < size; ++i) {
        SDL_memset(surface->pixels + i, displayBuffer[i], 1);
    }
}

static SDL_Color colors[2] = {{0,   0,   0,   255},
                              {255, 255, 255, 255}};

void render(SDLDisplay *sdlDisplay, const uint8_t * displayBuffer, size_t size) {
    SDL_Surface *surface = SDL_CreateRGBSurfaceWithFormat(SDL_SWSURFACE, CHIP8_DISPLAY_WIDTH, CHIP8_DISPLAY_HEIGHT, 1, SDL_PIXELFORMAT_INDEX1MSB);
    SDL_SetPaletteColors(surface->format->palette, colors, 0, 2);

    // Initialize renderer color white for the background
    SDL_SetRenderDrawColor(sdlDisplay->renderer, 0xFF, 0xFF, 0xFF, 0xFF);

    // Clear screen
    SDL_RenderClear(sdlDisplay->renderer);

    render_display_to_surface(surface, displayBuffer, size);
    SDL_Texture *screen_texture = SDL_CreateTextureFromSurface(sdlDisplay->renderer, surface);
    SDL_RenderCopy(sdlDisplay->renderer, screen_texture, NULL, NULL);

    // Update screen
    SDL_RenderPresent(sdlDisplay->renderer);
    SDL_FreeSurface(surface);
}

void free_sdl_display(SDLDisplay *sdlDisplay) {
    SDL_DestroyRenderer(sdlDisplay->renderer);
    SDL_DestroyWindow(sdlDisplay->window);
    SDL_Quit();
}
