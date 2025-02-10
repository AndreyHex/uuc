#include "../include/bytecode.h"
#include "../include/vm.h"
#include <stdint.h>
#include <stdio.h>

void vm_tick(VM *vm);
void vm_advance(VM *vm);
int uuc_type_check(Value val, UucType type, const char *msg);

VM vm_init(Slice slice) {
    VM vm = {
        .slice = slice,
        .ip = slice.codes,
        .ii = 0,
        .value_stack = stack_init(32),
    };
    return vm;
}

void vm_run(VM *vm) { 
    while(vm->ii < vm->slice.size) {
        vm_tick(vm);
        vm_advance(vm);
    }
}

void vm_advance(VM *vm) {
    vm->ip++;
    vm->ii++;
}

void vm_tick(VM *vm) {
    uint8_t ip = *vm->ip;
    Values *stack = &vm->value_stack;
    Values *constants = &vm->slice.constants;
    switch(ip) {
        case OP_CONSTANT: {
            vm_advance(vm);
            uint8_t index = *vm->ip;
            stack_push(stack, list_get(constants, index));
            break;
        }
        case OP_CONSTANT_16: {
            uint8_t left = 000; // next index 
            uint8_t right = 000; // next next index byte
            uint16_t index = left | (right << 8);
            stack_push(stack, list_get(constants, index));
            break;
        }
        case OP_NOT: {
            Value v = stack_pop(stack);
            uuc_type_check(v, TYPE_BOOL, "Cannot apply logical not to non bool type.");
            v.as.uuc_bool = !v.as.uuc_bool;
            stack_push(stack, v);
            break;
        }
        case OP_NEGATE: {
            Value v = stack_pop(stack);
            uuc_type_check(v, TYPE_INT, "Cannot negate non number type.");
            stack_push(stack, type_int(-(stack_pop(stack).as.uuc_int)));
            break;
        }
        case OP_ADD: {
            Value b = stack_pop(stack);
            Value a = stack_pop(stack);
            LOG_TRACE("a + b => %f + %f\n", a, b);
            stack_push(stack, type_int(a.as.uuc_int + b.as.uuc_int));
            break;
        }
        case OP_SUBSTRACT: {
            Value b = stack_pop(stack);
            Value a = stack_pop(stack);
            LOG_TRACE("a - b => %f - %f\n", a, b);
            stack_push(stack, type_int(a.as.uuc_int - b.as.uuc_int));
            break;
        }
        case OP_MULTIPLY: {
            Value b = stack_pop(stack);
            Value a = stack_pop(stack);
            LOG_TRACE("a * b => %f * %f\n", a, b);
            stack_push(stack, type_int(a.as.uuc_int * b.as.uuc_int));
            break;
        }
        case OP_DIVIDE: {
            Value b = stack_pop(stack);
            Value a = stack_pop(stack);
            LOG_TRACE("a / b => %f / %f\n", a, b);
            stack_push(stack, type_int(a.as.uuc_int / b.as.uuc_int));
            break;
        }
        case OP_RETURN: {
            printf("OP_RETURN\n");
            // printf("%f\n", stack_pop(stack));
            break;
        }
        default: printf("DEFAULT\n");
    }
    #if defined (UUC_LEVEL_TRACE)
    stack_print(&vm->value_stack);
    #endif
}

int uuc_type_check(Value val, UucType type, const char *msg) {
    if(val.type != type) {
        LOG_ERROR("%s\n", msg);
        exit(1);
    }
    return 0;
}
