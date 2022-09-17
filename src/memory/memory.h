#ifndef CHIP8_MEMORY_H
#define CHIP8_MEMORY_H

#include <stdint.h>
#include "../common.h"


/**
 * Memory is a 4kb( 4096 bytes)
 */
typedef struct {
    uint8_t bytes[MEMORY_SIZE];
} Memory;

Memory *create_memory();

void init_memory(Memory *memory);

uint16_t memory_opcode_at(Memory *mem, size_t index);

void memory_set(Memory *mem, size_t index, uint8_t value);

/**
 *
 * @param mem memory to memory_copy to
 * @param memIndex index in memory
 * @param buffer buffer to memory_copy
 * @param bufferIndex index to start copying from
 * @param size size of buffer
 * @return 1 if copied and 0 if could not memory_copy
 */
uint8_t memory_load(Memory *mem, size_t memIndex, const void *buffer, size_t bufferIndex, size_t size);


uint8_t memory_copy(void *buffer, size_t bufferIndex, const Memory *mem, size_t memIndex, size_t size);

void free_memory(Memory *memory);

#endif //CHIP8_MEMORY_H
