#ifndef uuc_vm_h
#define uuc_vm_h

#include "uuc_bytecode.h"
#include "uuc_type.h"
#include "uuc_val_table.h"
#include "uuc_result.h"
#include "uuc_function.h"
#include <stdint.h>

#define MAX_CALL_FRAMES 225

typedef struct {
    UucFunction *function;
    uint8_t *ip;
    uint32_t ii;
    uint32_t stack_offset;
} UucCallFrame;

typedef struct {
    UucFunction *main;
    ValueStack value_stack;
    UucValTable global_table;
    UucCallFrame call_frames[MAX_CALL_FRAMES];
    uint8_t frames_size;
} VM;

VM uuc_vm_init(UucFunction *main);
UucResult uuc_vm_run(VM *vm);

void uuc_vm_free(VM *vm);

void uuc_vm_dump(VM *vm);


#undef MAX_CALL_FRAMES

#endif
