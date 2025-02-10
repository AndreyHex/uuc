#include "../include/collection.h"
#include "../include/memory.h"
#include <stdint.h>
#include <stdlib.h>

ValueStack stack_init(uint32_t initial_capacity) {
    if(initial_capacity <= 0) {
        LOG_ERROR("Stack initial capacity cannot be less than 1.\n");
        exit(1);
    }
    Value *p = (Value*) malloc(initial_capacity * sizeof(Value));
    return (ValueStack){
        .head = p,
        .tail = NULL,
        .capacity = initial_capacity,
        .size = 0
    };
}

void stack_push(ValueStack *stack, Value val) {
    if(stack->size == stack->capacity) {
        uint8_t new_cap = stack->capacity * 2;
        LOG_TRACE("Growing stack size from %d to %d\n.", stack->size, new_cap);
        stack->head = INCREASE_ARRAY(Value, stack->head, stack->capacity, new_cap);
        stack->capacity = new_cap;
    }
    stack->head[stack->size] = val;
    stack->tail = &stack->head[stack->size];
    stack->size++;
}

Value stack_pop(ValueStack *stack) {
    if(stack->size == 0) {
        LOG_ERROR("Popping empty stack!\n");
        return type_null();
    }
    Value r = *stack->tail;
    stack->size--;
    stack->tail = &stack->head[stack->size - 1];
    return r; 
}

Value stack_peek(ValueStack *stack) {
    if(stack->size == 0) {
        LOG_ERROR("Peeking in empty stack!\n");
        return type_null();
    }
    return *stack->tail;
}

void stack_print(ValueStack *stack) {
    printf("Stack dump. Capacity: %d Size: %d. Content: [", stack->capacity, stack->size);
    for(int i = 0; i < stack->size; i++) {
        Value v = stack->head[i];
        switch(v.type) {
            case TYPE_INT: printf("%d, ", v.as.uuc_int); break;
            case TYPE_DOUBLE: printf("%.3f, ", v.as.uuc_double); break;
            case TYPE_BOOL: printf("%s, ", v.as.uuc_bool ? "true" : "false"); break;
            case TYPE_NULL: printf("null"); break;
            default: break;
        }
    }
    printf("]\n");
}

void stack_free(Values *stack) {
    free(stack->head);
}
