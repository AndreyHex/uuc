#ifndef uuc_string_stack_h
#define uuc_string_stack_h

#include <cstdint>
#include <stdint.h>

typedef struct {
    char **head;
    uint64_t capacity;
    uint64_t size;
    uint64_t string_size;
} StringStack;

#endif
