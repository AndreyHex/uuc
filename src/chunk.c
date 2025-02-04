#include "include/chunk.h"
#include <stdint.h>

void init_chunk(Chunk *chunk) {
    chunk->count = 0;
    chunk->capacity = 16;
    chunk->code = NULL;
}

void wire_chunk(Chunk *chunk, uint8_t byte) {
    if (chunk->capacity < chunk->count + 1) {
        int old_capacity = chunk->capacity;
        chunk->capacity = INCREASE_CAPACITY(old_capacity);
        chunk->code =
            INCREASE_ARRAY(uint8_t, chunk->code, old_capacity, chunk->capacity);
    }
    chunk->code[chunk->count] = byte;
    chunk->count++;
}

void free_chunk(Chunk *chunk) {
    FREE_ARRAY(uint8_t, chunk->code, 0);
    init_chunk(chunk);
}
