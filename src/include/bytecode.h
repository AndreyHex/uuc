#ifndef uuc_bytecode_h
#define uuc_bytecode_h

#include <stdint.h>
typedef enum {
    OP_CONSTANT,
    OP_RETURN,
} OpCode;

typedef struct {
    uint8_t size;
    uint8_t capacity;
    OpCode *codes;
} Slice;


#endif
