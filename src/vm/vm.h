#ifndef CHIP8_VM_H
#define CHIP8_VM_H

#include "../display/display.h"
#include "../memory/memory.h"
#include <stdint.h>

#define VM_STACK_SIZE 16
#define CHIP8_PROGRAM_START 0x200
#define OPCODE_SIZE  sizeof(uint16_t)
#define END_OPCODE 0xFFFF

typedef struct {
    Display *display;
    Memory *memory;
    uint16_t stack[VM_STACK_SIZE];
    uint8_t registers[16];
    uint8_t keyboard[16];
    uint8_t sound_register;
    uint8_t timer_register;
    uint16_t memory_register; // l register
    uint16_t pc;
    uint16_t sp;
} VM;

void init_vm(VM *vm);

/**
 * decrement timer and sound register
 * @param VM
 */
void tick(VM*vm);

/**
 * emulate the cpu
 * fetch decode excute
 * @param vm
 */
void cycle(VM * vm);

void free_vm(VM *vm);

#endif //CHIP8_VM_H
