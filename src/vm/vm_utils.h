#ifndef CHIP8_VM_UTILS_H
#define CHIP8_VM_UTILS_H

#define NEXT_OPCODE_ADDR_FROM(pc, count) (pc + OPCODE_SIZE * count)

#define ADDR(opcode) (opcode & 0x0FFF)
#define VALUE(opcode) (opcode & 0x00FF)
#define FIRST_REGISTER(opcode) ((opcode & 0x0F00) >> 8)
#define SECOND_REGISTER(opcode) ((opcode & 0x00F0) >> 4)
#define LAST_DIGIT(opcode) (opcode & 0x000F)
#define LAST_2DIGITS(opcode) VALUE(opcode)


#endif //CHIP8_VM_UTILS_H
