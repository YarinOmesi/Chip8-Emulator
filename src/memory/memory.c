#include "memory.h"
#include <stdlib.h>
#include <string.h>

Memory *create_memory() {
    return (Memory *) malloc(sizeof(Memory));
}

void init_memory(Memory *memory) {
    for (int i = 0; i < MEMORY_SIZE; ++i) {
        memory->bytes[i] = 0;
    }
}

uint16_t memory_opcode_at(Memory *mem, size_t index) {
    return (mem->bytes[index] << 8 | mem->bytes[index +1]);
}

void memory_set(Memory *mem, size_t index, uint8_t value) {
    mem->bytes[index] = value;
}


uint8_t memory_load(Memory *mem, size_t memIndex, const void *buffer, size_t bufferIndex, size_t size) {
    // check that index in memory not out of range
    if (memIndex >= MEMORY_SIZE) {
        return 0;
    }
    // check that buffer not bigger than memory
    if (size > MEMORY_SIZE) {
        return 0;
    }
    // check that index in buffer is valid
    if (bufferIndex >= size) {
        return 0;
    }

    memcpy(mem->bytes + memIndex, buffer + bufferIndex, size);
    return 1;
}

void free_memory(Memory *memory) {
    free(memory);
}

uint8_t memory_copy(void *buffer, size_t bufferIndex, const Memory *mem, size_t memIndex, size_t size) {
    // check that index in memory not out of range
    if (memIndex >= MEMORY_SIZE) {
        return 0;
    }
    // check that buffer not bigger than memory
    if (size > MEMORY_SIZE) {
        return 0;
    }
    // check that index in buffer is valid
    if (bufferIndex >= size) {
        return 0;
    }

    memcpy(buffer + bufferIndex, mem->bytes + memIndex, size);
    return 1;
}

