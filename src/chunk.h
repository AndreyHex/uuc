#ifndef chunk_h
#define chunk_h

#include "memory.h"
#include <stdint.h>

#define INCREASE_CAPACITY(capacity) ((capacity) < 16 ? 16 : (capacity) * 2)
#define INCREASE_ARRAY(type, pointer, old_capacity, new_capacity)              \
    (type *)reallocate(pointer, sizeof(type) * (old_capacity),                 \
                       sizeof(type) * (new_capacity))

typedef enum {
    OP_RETURN,
} OpCode;

typedef struct {
    int count;
    int capacity;
    uint8_t *code;
} Chunk;

void init_chunk(Chunk *chunk);
void wire_chunk(Chunk *chunk, uint8_t byte);

#endif
