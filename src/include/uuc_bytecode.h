#ifndef uuc_bytecode_h
#define uuc_bytecode_h

#include <stdint.h>
#include "uuc_collection.h"

typedef enum {
    OP_CONSTANT,
    OP_CONSTANT_16,

    OP_DEFINE_GLOBAL, // TOOD: 16 bit version ?
    OP_GET_GLOBAL,

    OP_TRUE,
    OP_FALSE,
    OP_NULL,

    OP_NEGATE,
    OP_NOT,

    OP_ADD,
    OP_SUBSTRACT,
    OP_MULTIPLY,
    OP_DIVIDE,

    OP_EQ,
    OP_NE,
    OP_GT,
    OP_GTE,
    OP_LT,
    OP_LTE,

    OP_RETURN,
    OP_POP,
} OpCode;

typedef struct {
    uint32_t size;
    uint32_t capacity;
    uint8_t *codes;
    ValueList constants;
} Slice;

Slice slice_init(uint32_t initial_capacity);
void slice_push_code(OpCode code, Slice *slice);
// just adds constant to store
uint64_t slice_register_constant(Value value, Slice *slice);
// adds constant and OP_CONSTANT opcode
uint64_t slice_push_constant(Value value, Slice *slice);

void slice_print(Slice *slice);
const char* opcode_name(OpCode opcode);

#endif
