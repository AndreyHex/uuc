#include "../include/bytecode.h"
#include "../include/vm.h"
#include <stdint.h>
#include <stdio.h>

void vm_tick(VM *vm);
void vm_advance(VM *vm);

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
            printf("OP_CONSTANT\n");
            vm_advance(vm);
            uint8_t next_byte = *vm->ip;
            stack_push(stack, list_get(constants, next_byte));
            break;
        }
        case OP_CONSTANT_16: {
            printf("OP_CONSTANT_16\n");
            uint8_t left = 000; // next byte
            uint8_t right = 000; // next next byte
            uint16_t index = left | (right << 8);
            stack_push(stack, list_get(constants, index));
            break;
        }
        case OP_NEGATE: {
            printf("OP_NEGATE\n");
            stack_push(stack, - stack_pop(stack));
            break;
        }
        case OP_ADD: {
            printf("OP_ADD\n");
            Value a = stack_pop(stack);
            Value b = stack_pop(stack);
            stack_push(stack, a + b);
            break;
        }
        case OP_SUBSTRACT: {
            printf("OP_SUBSTRACT\n");
            Value a = stack_pop(stack);
            Value b = stack_pop(stack);
            stack_push(stack, a - b);
            break;
        }
        case OP_MULTIPLY: {
            printf("OP_MULTIPLY\n");
            Value a = stack_pop(stack);
            Value b = stack_pop(stack);
            stack_push(stack, a * b);
            break;
        }
        case OP_DIVIDE: {
            printf("OP_DIVIDE\n");
            Value a = stack_pop(stack);
            Value b = stack_pop(stack);
            stack_push(stack, a / b);
            break;
        }
        case OP_RETURN: {
            printf("OP_RETURN\n");
            printf("%f\n", stack_pop(stack));
            break;
        }
        default: printf("DEFAULT\n");
    }
    stack_print(&vm->value_stack);
}
