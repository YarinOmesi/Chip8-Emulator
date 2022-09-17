#include <stdio.h>
#include "vm.h"
#include "../debug.h"
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include "vm_utils.h"
#ifdef PRINT_OPCODE
#include "vm_debug.h"
#endif

void init_vm(VM *vm) {
    // init display
    vm->display = create_display();
    init_display(vm->display);

    // init memory
    vm->memory = create_memory();
    init_memory(vm->memory);

    // load sprites to memory
    uint8_t sprites[16][5] = {
            //{0xF0,0x90,0x90,0x90,0xF0}, // 0
            {0xF0,0x90,0x90,0x90,0xF0}, // 0
            {0x20,0x60,0x20,0x20,0x70}, // 1
            {0xF0,0x10,0xF0,0x80,0xF0},// 2
            {0xF0,0x10,0xF0,0x10,0xF0},// 3
            {0x90,0x90,0xF0,0x10,0x10},// 4
            {0xF0,0x80,0xF0,0x10,0xF0},// 5
            {0xF0,0x80,0xF0,0x90,0xF0},// 6
            {0xF0,0x10,0x20,0x40,0x40},// 7
            {0xF0,0x90,0xF0,0x90,0xF0},// 8
            {0xF0,0x90,0xF0,0x10,0xF0},// 9
            {0xF0,0x90,0xF0,0x90,0x90},// A
            {0xE0,0x90,0xE0,0x90,0xE0},// B
            {0xF0,0x80,0x80,0x80,0xF0},// C
            {0xE0,0x90,0x90,0x90,0xE0},// D
            {0xF0,0x80,0xF0,0x80,0xF0},// E
            {0xF0,0x80,0xF0,0x80,0x80},// F
    };
    for (int i = 0; i < SPRITES_COUNT; ++i) {
        int memIndex = SPRITES_START_ADDRESS + (i * SPRITE_SIZE_IN_BYTES);
        memory_load(vm->memory, memIndex, sprites[i], 0, SPRITE_SIZE_IN_BYTES);
    }

    // init keyboard
    memset(vm->keyboard,0,16);

    // init stack
    for (int i = 0; i < VM_STACK_SIZE; ++i) {
        vm->stack[i] = 0;
    }
    // init registers
    for (int i = 0; i < 16; ++i) {
        vm->registers[i] = 0;
    }
    vm->memory_register = 0;
    vm->timer_register = 0;
    vm->sound_register = 0;

    vm->pc = 0;
    vm->sp = 0;

    srand(time(NULL));
}



/**
 *
 * @param vm pointer to the vm
 * @param opcode opcode to run
 * @return new program counter ( pc )
 */
static uint16_t execute_opcode(VM *vm, uint16_t opcode) {
    uint16_t left_most = opcode & 0xF000;
    if (opcode == 0x00E0) { // Clear Display
        clear_display(vm->display);
    } else if (opcode == 0x00EE) { // Return
        return NEXT_OPCODE_ADDR_FROM(vm->stack[--vm->sp], 1);
    } else if (left_most == 0x1000) { // Jump
        return ADDR(opcode);
    } else if (left_most == 0x2000) { // Call subroutine
        vm->stack[vm->sp++] = vm->pc;
        return ADDR(opcode);
    } else if (left_most == 0x3000) { // Skip If Equal
        if (vm->registers[FIRST_REGISTER(opcode)] == VALUE(opcode)) {
            return NEXT_OPCODE_ADDR_FROM(vm->pc, 2);
        }
    } else if (left_most == 0x4000) { // Skip If Not Equal
        if (vm->registers[FIRST_REGISTER(opcode)] != VALUE(opcode)) {
            return NEXT_OPCODE_ADDR_FROM(vm->pc, 2);
        }
    } else if (left_most == 0x5000) { // Skip If Registers Equal
        if (vm->registers[FIRST_REGISTER(opcode)] == vm->registers[SECOND_REGISTER(opcode)]) {
            return NEXT_OPCODE_ADDR_FROM(vm->pc, 2);
        }
    } else if (left_most == 0x6000) { // Set Register Value
        vm->registers[FIRST_REGISTER(opcode)] = VALUE(opcode);
    } else if (left_most == 0x7000) { // Add Value To Register and Store in Register
        vm->registers[FIRST_REGISTER(opcode)] += VALUE(opcode);
    } else if (left_most == 0x8000) {
#pragma region opcodes_8
        if (LAST_DIGIT(opcode) == 0) {
            vm->registers[FIRST_REGISTER(opcode)] = vm->registers[SECOND_REGISTER(opcode)];
        } else if (LAST_DIGIT(opcode) == 1) {
            vm->registers[FIRST_REGISTER(opcode)] |= vm->registers[SECOND_REGISTER(opcode)];
        } else if (LAST_DIGIT(opcode) == 2) {
            vm->registers[FIRST_REGISTER(opcode)] &= vm->registers[SECOND_REGISTER(opcode)];
        } else if (LAST_DIGIT(opcode) == 3) {
            vm->registers[FIRST_REGISTER(opcode)] ^= vm->registers[SECOND_REGISTER(opcode)];
        } else if (LAST_DIGIT(opcode) == 4) {
            uint8_t first_register_number = FIRST_REGISTER(opcode);
            uint8_t second_register_value = vm->registers[SECOND_REGISTER(opcode)];
            vm->registers[0xF] = (second_register_value > 0 && vm->registers[first_register_number] > INT16_MAX - second_register_value);
            vm->registers[first_register_number] += second_register_value;
        } else if (LAST_DIGIT(opcode) == 5) {
            uint8_t first_register_number = FIRST_REGISTER(opcode);
            uint8_t second_register_value = vm->registers[SECOND_REGISTER(opcode)];

            vm->registers[0xF] = (vm->registers[first_register_number] > second_register_value);
            vm->registers[first_register_number] -= second_register_value;
        }
#pragma endregion opcodes_8
    } else if (left_most == 0x9000) { // Skip If Registers Not Equal
        if (vm->registers[FIRST_REGISTER(opcode)] != vm->registers[SECOND_REGISTER(opcode)]) {
            return NEXT_OPCODE_ADDR_FROM(vm->pc, 2);
        }
    } else if (left_most == 0xA000) { // memory_set value of memory register
        vm->memory_register = ADDR(opcode);
    } else if (left_most == 0xB000) { // Jump to location nnn + V0.
        return vm->registers[0] + ADDR(opcode);
    } else if (left_most == 0xC000) {
        vm->registers[FIRST_REGISTER(opcode)] = (rand() & 255) && VALUE(opcode);
    } else if (left_most == 0xD000) {// DXYN
        // Display n-byte sprite starting at memory location I memory_at (Vx, Vy) memory_set VF = collision.
        uint8_t row = vm->registers[FIRST_REGISTER(opcode)];
        uint8_t col = vm->registers[SECOND_REGISTER(opcode)];
        uint8_t sprite_size = LAST_DIGIT(opcode);

        uint8_t had_collision = display_sprite(vm->display, row, col,(((uint8_t *) vm->memory->bytes) + vm->memory_register), sprite_size);
        vm->registers[0xF] = had_collision;
#ifdef DISPLAY_DUMP
        display_dump(vm);
#endif
    } else if(left_most == 0xE000){ //skip next instruction if key with the value of Vx is not pressed.
        uint8_t register_value = vm->registers[FIRST_REGISTER(opcode)];
        if(register_value < 0xF && !vm->keyboard[register_value]){
            return NEXT_OPCODE_ADDR_FROM(vm->pc, 2);
        }
    }else if (left_most == 0xF000) {
#pragma region F_opcodes
            if(LAST_2DIGITS(opcode) == 0x07){ // Set Vx = delay timer value. VX = DT
                vm->registers[FIRST_REGISTER(opcode)] = vm->timer_register;
            }else if(LAST_2DIGITS(opcode) == 0x0A){ // Wait for a key press, store the value of the key in Vx.
                uint8_t waiting = 1;
                while(waiting){
                    for (int i = 0; waiting && i < 16; ++i) {
                        if (vm->keyboard[i]){
                            vm->registers[FIRST_REGISTER(opcode)] = i;
                            waiting = 0;
                        }
                    }
                }
            }else if(LAST_2DIGITS(opcode) == 0x15){ // Set delay timer = Vx. DT
                vm->timer_register = vm->registers[FIRST_REGISTER(opcode)];
            }else if(LAST_2DIGITS(opcode) == 0x18){ // Set sound timer = Vx. ST
                vm->sound_register = vm->registers[FIRST_REGISTER(opcode)];
            }else if (LAST_2DIGITS(opcode) == 0x1E) { //FX1E I += VX
                vm->memory_register += vm->registers[FIRST_REGISTER(opcode)];
            }else if(LAST_2DIGITS(opcode) == 0x29){ // Set I = location of sprite for digit Vx.
                uint8_t register_value = vm->registers[FIRST_REGISTER(opcode)];
                if (register_value < 16){
                    vm->memory_register = (SPRITES_START_ADDRESS + (register_value * SPRITE_SIZE_IN_BYTES));
                }else{
                    vm->memory_register = 0;
                }
            }else if (LAST_2DIGITS(opcode) == 0x33) { //FX33 Store BCD representation of Vx in memory locations I, I+1, and I+2.
                uint8_t value = vm->registers[FIRST_REGISTER(opcode)];
                memory_set(vm->memory, vm->memory_register + 0, value / 100); // 100
                memory_set(vm->memory, vm->memory_register + 1, (value / 10) % 10);// 10
                memory_set(vm->memory, vm->memory_register + 2, value % 10); // 1
            } else if (LAST_2DIGITS(opcode) == 0x55) { //FX55 Store registers V0 through Vx in memory starting memory_at location I.
                memory_load(vm->memory, vm->memory_register, vm->registers, 0, FIRST_REGISTER(opcode));
            } else if (LAST_2DIGITS(opcode) == 0x65) { // Read registers V0 through Vx from memory starting memory_at location I.
                memory_copy(vm->registers, 0, vm->memory, vm->memory_register, FIRST_REGISTER(opcode) + 1);
            }
#pragma endregion F_opcodes

    }else{
        printf("UNKNOWN OPCODE %x\n",opcode);
        exit(1);
    }

    return NEXT_OPCODE_ADDR_FROM(vm->pc, 1);
}

void free_vm(VM *vm) {
    free_display(vm->display);
    free_memory(vm->memory);
}

void tick(VM *vm) {
    if (vm->timer_register > 0){
        --vm->timer_register;
    }
    if (vm->sound_register > 0){
        --vm->sound_register;
    }
}

void cycle(VM *vm) {
    uint16_t opcode = memory_opcode_at(vm->memory, vm->pc);
    vm->pc=execute_opcode(vm, opcode);
#ifdef PRINT_OPCODE
    print_opcode(vm,opcode);
#endif
}

