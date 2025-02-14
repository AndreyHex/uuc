#ifndef uuc_vm_h
#define uuc_vm_h

#include "bytecode.h"
#include <stdint.h>

typedef enum {
    UUC_OK,
    UUC_COMP_ERROR,
    UUC_RUNTIME_ERROR,
} ExeResult;

typedef struct {
    Slice slice;
    uint8_t *ip; // instruction ptr
    uint64_t ii; // instruction index
    ValueStack value_stack;
} VM;

VM vm_init(Slice slice);
ExeResult vm_run(VM *vm);

#endif
