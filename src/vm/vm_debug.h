#ifndef CHIP8_VM_DEBUG_H
#define CHIP8_VM_DEBUG_H
#include "vm.h"
#include <stdlib.h>

void print_opcode(VM *vm, uint16_t opcode);

static void print_debug(VM *vm);

#endif //CHIP8_VM_DEBUG_H
