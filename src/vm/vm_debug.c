#include "vm_debug.h"
#include <stdio.h>
#include "vm_utils.h"
#include "string.h"

void print_opcode(VM *vm, uint16_t opcode) {
    uint16_t left_most = opcode & 0xF000;
    if (opcode == 0x00E0) { // Clear Display
        printf("ClearDisplay()\n");
    } else if (opcode == 0x00EE) { // Return
        printf("Return To %d\n",vm->stack[vm->sp -1]);
    } else if (left_most == 0x1000) { // Jump
        printf("Jump %d\n",ADDR(opcode));
    } else if (left_most == 0x2000) { // Call subroutine
        printf("Call(%d)\n",ADDR(opcode));
    } else if (left_most == 0x3000) { // Skip If Equal
        printf("SkipIfEqual(V%d, %d)\n",FIRST_REGISTER(opcode), VALUE(opcode));
    } else if (left_most == 0x4000) { // Skip If Not Equal
        printf("SkipIfNotEqual(V%d, %d)\n",FIRST_REGISTER(opcode), VALUE(opcode));
    } else if (left_most == 0x5000) { // Skip If Registers Equal
        printf("SkipIfEqual(V%d, V%d)\n",FIRST_REGISTER(opcode), SECOND_REGISTER(opcode));
    } else if (left_most == 0x6000) { // Set Register Value
        printf("V%d = %d\n", FIRST_REGISTER(opcode), VALUE(opcode));
    } else if (left_most == 0x7000) { // Add Value To Register and Store in Register
        printf("V%d += %d\n", FIRST_REGISTER(opcode), VALUE(opcode));
    } else if (left_most == 0x8000) {
#pragma region opcodes_8
        if (LAST_DIGIT(opcode) == 0) {
            printf("V%d = V%d\n", FIRST_REGISTER(opcode), SECOND_REGISTER(opcode));
        } else if (LAST_DIGIT(opcode) == 1) {
            printf("V%d |= V%d\n", FIRST_REGISTER(opcode), SECOND_REGISTER(opcode));
        } else if (LAST_DIGIT(opcode) == 2) {
            printf("V%d &= V%d\n", FIRST_REGISTER(opcode), SECOND_REGISTER(opcode));
        } else if (LAST_DIGIT(opcode) == 3) {
            printf("V%d ^= V%d\n", FIRST_REGISTER(opcode), SECOND_REGISTER(opcode));
        } else if (LAST_DIGIT(opcode) == 4) {
            printf("V%d += V%d, VF = %d\n", FIRST_REGISTER(opcode), SECOND_REGISTER(opcode), vm->registers[0xF]);
        } else if (LAST_DIGIT(opcode) == 5) {
            printf("V%d -= V%d, VF = %d\n", FIRST_REGISTER(opcode), SECOND_REGISTER(opcode), vm->registers[0xF]);
        }
#pragma endregion opcodes_8
    } else if (left_most == 0x9000) { // Skip If Registers Not Equal
        printf("SkipIfNotEqual(V%d, V%d)\n",FIRST_REGISTER(opcode), SECOND_REGISTER(opcode));
    } else if (left_most == 0xA000) { // memory_set value of memory register
        printf("I = %d\n",ADDR(opcode));
    } else if (left_most == 0xB000) { // Jump to location nnn + V0.
        printf("Jump V0+%d, %d\n",ADDR(opcode), vm->registers[0] + ADDR(opcode));
    } else if (left_most == 0xC000) {
        printf("V%d = rnd & %d, %d\n", FIRST_REGISTER(opcode),VALUE(opcode), (rand() & 255) && VALUE(opcode));
    } else if (left_most == 0xD000) {// DXYN
        // Display n-byte sprite starting at memory location I memory_at (Vx, Vy) memory_set VF = collision.
        uint8_t row = vm->registers[FIRST_REGISTER(opcode)];
        uint8_t col = vm->registers[SECOND_REGISTER(opcode)];
        uint8_t sprite_size = LAST_DIGIT(opcode);
        printf("Display %d bytes at (%d, %d), collision = %d\n", sprite_size, row,col, vm->registers[0xF]);
    } else if(left_most == 0xE000){ //skip next instruction if key with the value of Vx is not pressed.
        uint8_t register_value = vm->registers[FIRST_REGISTER(opcode)];
        printf("SkipIfNotPressed(V%d), key=%d , isPressed=%d\n", FIRST_REGISTER(opcode), register_value,!vm->keyboard[register_value]);
    }else if (left_most == 0xF000) {
#pragma region F_opcodes
        if(LAST_2DIGITS(opcode) == 0x07){ // Set Vx = delay timer value. VX = DT
            printf("V%d = timer, %d\n", FIRST_REGISTER(opcode), vm->timer_register);
        }else if(LAST_2DIGITS(opcode) == 0x0A){ // Wait for a key press, store the value of the key in Vx.
            printf("WaitForKey(&V%d)\n", FIRST_REGISTER(opcode));
        }else if(LAST_2DIGITS(opcode) == 0x15){ // Set delay timer = Vx. DT
            printf("Timer = V%d, %d\n", FIRST_REGISTER(opcode), vm->registers[FIRST_REGISTER(opcode)]);
        }else if(LAST_2DIGITS(opcode) == 0x18){ // Set sound timer = Vx. ST
            printf("Sound = V%d, %d\n", FIRST_REGISTER(opcode), vm->registers[FIRST_REGISTER(opcode)]);
        }else if (LAST_2DIGITS(opcode) == 0x1E) { //FX1E I += VX
            printf("I += V%d, %d\n", FIRST_REGISTER(opcode), vm->registers[FIRST_REGISTER(opcode)]);
        }else if(LAST_2DIGITS(opcode) == 0x29){ // Set I = location of sprite for digit Vx.
            uint8_t register_value = vm->registers[FIRST_REGISTER(opcode)];
            printf("I = SPRITE(V%d), digit = %x location=%x\n", FIRST_REGISTER(opcode), register_value,  vm->memory_register);
        }else if (LAST_2DIGITS(opcode) == 0x33) { //FX33 Store BCD representation of Vx in memory locations I, I+1, and I+2.
            uint8_t value = vm->registers[FIRST_REGISTER(opcode)];
            printf("StoreBcdOf(%x,[%d, %d, %d])\n", FIRST_REGISTER(opcode),value / 100, (value / 10) % 10, value % 10);
        } else if (LAST_2DIGITS(opcode) == 0x55) { //FX55 Store registers V0 through Vx in memory starting memory_at location I.
            printf("LoadRegisterFromMemory(%X)\n", vm->memory_register);
        } else if (LAST_2DIGITS(opcode) == 0x65) { // Read registers V0 through Vx from memory starting memory_at location I.
            printf("CopyRegisterToMemory(%x)\n", vm->memory_register);
        }
#pragma endregion F_opcodes
    }else{
        printf("UNKNOWN OPCODE %x\n",opcode);
        exit(1);
    }
}

/* ======================== DEBUG PRINT =============================== */
#define DEBUG_WINDOW_WIDTH 100
#define DEBUG_WINDOW_HEIGHT 50
#define DEBUG_WINDOW_SIZE (DEBUG_WINDOW_HEIGHT * DEBUG_WINDOW_WIDTH)
#define BUFFER_AT_PTR(row, col) ((screen_buffer) + ((row) * DEBUG_WINDOW_WIDTH + (col)))

static char screen_buffer[DEBUG_WINDOW_SIZE];


static void print_all_opcodes(VM *vm) {
    int row = 1, col = 2;
    uint16_t *opcode_ptr = (uint16_t *) (vm->memory->bytes + CHIP8_PROGRAM_START);
    sprintf(BUFFER_AT_PTR(0, col + 0), "INDX");
    sprintf(BUFFER_AT_PTR(0, col + 6), "OP");
    if (vm->pc >= CHIP8_PROGRAM_START) {
        sprintf(BUFFER_AT_PTR( row + (vm->pc - CHIP8_PROGRAM_START), 0), ">");
    }

    for (; *opcode_ptr != END_OPCODE; ++opcode_ptr, ++row) {
        sprintf(BUFFER_AT_PTR(row, col), "%04u", (((uint8_t *) opcode_ptr) - vm->memory->bytes));
        sprintf(BUFFER_AT_PTR(row, col + 5), "%x", *opcode_ptr);
    }

}

static void print_all_registers(VM *vm) {
    uint16_t register_number = 0;
    for (int side = 0; side < 2; side++)
    {
        int row = 0;
        int col = 20 + side * 7;
        for (int index = 0; index < 8; index++, ++register_number)
        {
            uint8_t registerValue = vm->registers[register_number];

            sprintf(BUFFER_AT_PTR(row, col), "V%x",register_number);
            sprintf(BUFFER_AT_PTR(row, col + 3), "%x",registerValue);
            row++;
        }
    }

}

static void print_debug(VM *vm) {
    memset(screen_buffer,' ', DEBUG_WINDOW_SIZE);
    for (int row = 0; row < DEBUG_WINDOW_HEIGHT; ++row) {
        *BUFFER_AT_PTR(row, DEBUG_WINDOW_WIDTH -1) = '\n';
    }
    print_all_opcodes(vm);
    print_all_registers(vm);


    fwrite(screen_buffer, sizeof(char), DEBUG_WINDOW_SIZE,stdout);
}
