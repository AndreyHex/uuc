#ifndef uuc_vm_h
#define uuc_vm_h

#include "bytecode.h"
#include "values.h"
#include <stdint.h>

typedef struct {
    Slice slice;
    uint8_t *ip; // instruction ptr
    uint64_t ii; // instruction index
    ValueStack value_stack;
} VM;

VM vm_init(Slice slice);
void vm_run(VM *vm);

#endif
