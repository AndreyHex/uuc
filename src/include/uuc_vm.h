#ifndef uuc_vm_h
#define uuc_vm_h

#include "uuc_bytecode.h"
#include "uuc_val_table.h"
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
    UucValTable global_table;
} VM;

VM vm_init(Slice slice);
ExeResult vm_run(VM *vm);

void uuc_vm_free(VM *vm);

void uuc_vm_dump(VM *vm);

#endif
