#ifndef uuc_vm_h
#define uuc_vm_h

#include "uuc_bytecode.h"
#include "uuc_type.h"
#include "uuc_val_table.h"
#include "uuc_result.h"
#include <stdint.h>

typedef struct {
    Slice slice;
    uint8_t *ip; // instruction ptr
    uint64_t ii; // instruction index
    ValueStack value_stack;
    UucValTable global_table;
} VM;

VM vm_init(Slice slice);
UucResult vm_run(VM *vm);

void uuc_vm_free(VM *vm);

void uuc_vm_dump(VM *vm);

#endif
