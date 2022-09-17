#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdint.h>
#include "vm/vm.h"
#include <time.h>
#include <unistd.h>
#include "debug.h"

#include "sdl_display/sdl_display.h"

#define MILLIS(now) (((int64_t) now.tv_sec) * 1000 + ((int64_t) now.tv_nsec) / 1000000)

size_t keycode_count = 16;
SDL_KeyCode keyboard_key_codes[] = {
        SDLK_0,
        SDLK_1,
        SDLK_2,
        SDLK_3,
        SDLK_4,
        SDLK_5,
        SDLK_6,
        SDLK_7,
        SDLK_8,
        SDLK_9,
        SDLK_a,
        SDLK_b,
        SDLK_c,
        SDLK_d,
        SDLK_e,
        SDLK_f,
};

static void load_from_file(VM *vm, const char *file_path) {
    FILE *game = fopen(file_path, "rb");
    if (game) {
        fread(vm->memory->bytes + CHIP8_PROGRAM_START, 1, MAX_GAME_SIZE, game);
    } else {
        printf("Cannot load game");
        exit(42);
    }
    fclose(game);
}

static void load_from_array(VM *vm, uint8_t *opcodes, size_t size) {
    memory_load(vm->memory, CHIP8_PROGRAM_START, opcodes, 0, size);
}


int main(int argc, char **argv) {
    if (argc == 1){
        printf("chip8 <game-path>\nGame Path is required");
        return 1;
    }
    else if (argc > 2){
        printf("chip8 <game-path>\nExpected 1 argument got %d",argc-1);
        return 1;
    }
    SDLDisplay display;
    VM vm;
    init_sdl_display(&display);
    init_vm(&vm);

    struct timespec prev_clock;
    clock_gettime(CLOCK_MONOTONIC, &prev_clock);

    load_from_file(&vm,argv[1]);
    vm.pc = CHIP8_PROGRAM_START;

    while (1) {
        SDL_Event e;
        SDL_PollEvent(&e);
        if (e.type == SDL_QUIT) {
            break;
        }
        if (e.type == SDL_KEYDOWN) {
            for (int i = 0; i < keycode_count; ++i) {
                if (e.key.keysym.sym == keyboard_key_codes[i]) {
                    vm.keyboard[i] = 1;
                    break;
                }
            }
        } else if (e.type == SDL_KEYUP) {
            for (int i = 0; i < keycode_count; ++i) {
                if (e.key.keysym.sym == keyboard_key_codes[i]) {
                    vm.keyboard[i] = 0;
                    break;
                }
            }
        }

        struct timespec now;
        clock_gettime(CLOCK_MONOTONIC, &now);

        cycle(&vm);

        if (vm.display->draw_flag) {
            render(&display, vm.display->buffer, DISPLAY_BUFFER_SIZE);
            vm.display->draw_flag = 0;
        }
        if (MILLIS(now) - MILLIS(prev_clock) >= TICK_MILLIS) {
            tick(&vm);
        }
        SDL_Delay(TICK_MILLIS);
    }
    free_vm(&vm);
    free_sdl_display(&display);
}