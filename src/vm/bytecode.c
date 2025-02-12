#include "../include/bytecode.h"
#include "../include/log.h"
#include "../include/memory.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <values.h>

const char* opcode_name(OpCode opcode);

Slice slice_init(uint32_t init_capacity) {
    if(init_capacity <= 0) {
        LOG_ERROR("Cannot create slice with initial capacity less than 1.\n");
        exit(1);
    }
    void *codes = malloc(init_capacity * sizeof(uint8_t));
    return (Slice) {
        .size = 0,
        .capacity = init_capacity,
        .codes = codes
    };
}

void slice_push_code(OpCode code, Slice *slice) {
    if(slice->size == slice->capacity) {
        uint32_t new_cap = slice->capacity * 2;
        LOG_TRACE("Growing slice capacity from %d to %d\n", slice->capacity, new_cap);
        slice->codes = INCREASE_ARRAY(uint8_t, slice->codes, slice->capacity, new_cap);
    }
    slice->codes[slice->size] = code;
    slice->size++;
}

void slice_push_constant(Value value, Slice *slice) {
    if(slice->size + 3 >= slice->capacity) { // + 3 instruction potentially
        uint32_t new_cap = slice->capacity * 2;
        LOG_TRACE("Growing slice capacity from %d to %d\n", slice->capacity, new_cap);
        slice->codes = INCREASE_ARRAY(uint8_t, slice->codes, slice->capacity, new_cap);
    }
    uint64_t index = list_push(&slice->constants, value);
    OpCode code;
    if(index <= UINT8_MAX) {
        LOG_TRACE("Pushing OP_CONSTANT code with value: %f\n", value);
        code = OP_CONSTANT;
    } else if (index <= UINT16_MAX) {
        LOG_TRACE("Pushing OP_CONSTANT_16 code with value: %f\n", value);
        code = OP_CONSTANT_16;
    } else {
        LOG_ERROR("Cannot push constant instruction when index in constant list is greater than %d. Index: %d\n", UINT16_MAX, slice->constants.size);
        exit(1);
    }
    slice->codes[slice->size] = code;
    slice->size++;
    if(code == OP_CONSTANT) {
        slice->codes[slice->size] = index;
        slice->size++;
    } else {
        slice->codes[slice->size] = index >> 8;
        slice->size++;
        slice->codes[slice->size] = index;
        slice->size++;
    }
}

void slice_print(Slice *slice) {
    printf("====== Bytecode slice dump ======\n");
    printf(" Bytes: %d\n", slice->size);
    printf(" Instructions: %d\n", slice->size - slice->constants.size);
    printf(" Constants: %d\n", slice->constants.size);
    printf("============ Content ============\n");
    for(int i = 0; i < slice->size; i++) {
        uint8_t code = slice->codes[i];
        printf("%4d | ", i);
        if(code == OP_CONSTANT) {
            uint8_t index = slice->codes[i + 1];
            Value v = slice->constants.head[index];
            printf("%3d:%s = %d:", code, opcode_name(code), index);
            type_print(v);
            i++;
        } else if(code == OP_CONSTANT_16) {
            LOG_ERROR("Unsupported index constant length: 16!\n");
        } else {
            printf("%3d:%s", code, opcode_name(code));
        }
        printf("\n");
    }
    printf("=================================\n");
}


const char *op_code_names[] = {
    "OP_CONSTANT",
    "OP_CONSTANT_16",

    "OP_TRUE",
    "OP_FALSE",
    "OP_NULL",

    "OP_NEGATE",
    "OP_NOT",

    "OP_ADD",
    "OP_SUBSTRACT",
    "OP_MULTIPLY",
    "OP_DIVIDE",

    "OP_EQ",
    "OP_NE",
    "OP_GT",
    "OP_GTE",
    "OP_LT",
    "OP_LTE",

    "OP_RETURN",
};

const char* opcode_name(OpCode opcode) {
    if(opcode > 16) return "UNKNOWN OP CODE";
    return op_code_names[opcode];
}
