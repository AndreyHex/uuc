#ifndef chunk_h
#define chunk_h

#include "memory.h"
#include <stdint.h>

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
