cmake_minimum_required(VERSION 3.22)
project(chip8 C)

set(CMAKE_C_STANDARD 11)

set(SDL2_LIBRARY C:/CLIBS/SDL2/lib/x86/SDL2.dll)
set(SDL2_INCLUDE_DIR C:/CLIBS/SDL2/include)
# Add SDL2 CMake modules
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/sdl2)

add_executable(chip8 src/main.c src/display/display.c src/display/display.h src/memory/memory.c src/memory/memory.h src/vm/vm.c src/vm/vm.h src/sdl_display/sdl_display.c src/sdl_display/sdl_display.h src/debug.h src/common.h src/debug.c src/vm/vm_debug.c src/vm/vm_debug.h src/vm/vm_utils.h)

find_package(SDL2 REQUIRED)
target_link_libraries(${PROJECT_NAME} SDL2::Main)

