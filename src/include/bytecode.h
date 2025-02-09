#ifndef uuc_bytecode_h
#define uuc_bytecode_h

#include <stdint.h>
#include "collection.h"

typedef enum {
    OP_CONSTANT,
    OP_CONSTANT_16,

    OP_NEGATE,
    OP_NOT,

    OP_ADD,
    OP_SUBSTRACT,
    OP_MULTIPLY,
    OP_DIVIDE,


    OP_RETURN,
} OpCode;

typedef struct {
    uint32_t size;
    uint32_t capacity;
    uint8_t *codes;
    ValueList constants;
} Slice;

Slice slice_init(uint32_t initial_capacity);
void slice_push_code(OpCode code, Slice *slice);
void slice_push_constant(double value, Slice *slice);

void slice_print(Slice *slice);
const char* opcode_name(OpCode opcode);

#endif
