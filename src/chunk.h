#ifndef chunk_h
#define chunk_h

#include "memory.h"
#include <stdint.h>

#define INCREASE_CAPACITY(capacity) ((capacity) < 16 ? 16 : (capacity) * 2)
#define INCREASE_ARRAY(type, ptr, old_capacity, new_capacity)                  \
    (type *)reallocate(ptr, sizeof(type) * (old_capacity),                     \
                       sizeof(type) * (new_capacity))
#define FREE_ARRAY(type, ptr, old_capacity)                                    \
    reallocate(ptr, sizeof(type) * (old_capacity), 0)

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
void free_chunk(Chunk *chunk);

#endif
