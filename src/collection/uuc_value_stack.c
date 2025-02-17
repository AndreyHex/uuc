#include "../include/uuc_collection.h"
#include "../include/uuc_memory.h"
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
        .capacity = initial_capacity,
        .size = 0,
    };
}

void stack_push(ValueStack *stack, Value val) {
    if(stack->size == stack->capacity) {
        size_t new_cap = stack->capacity * 2;
        LOG_TRACE("Growing stack size from %d to %ld\n.", stack->size, new_cap);
        stack->head = INCREASE_ARRAY(Value, stack->head, stack->capacity, new_cap);
        stack->capacity = new_cap;
    }
    stack->head[stack->size] = val;
    stack->size++;
}

Value stack_pop(ValueStack *stack) {
    if(stack->size == 0) {
        LOG_ERROR("Popping empty stack!\n");
        return uuc_val_null();
    }
    stack->size--;
    Value r = stack->head[stack->size];
    return r; 
}

Value stack_peek(ValueStack *stack) {
    if(stack->size == 0) {
        LOG_ERROR("Peeking in empty stack!\n");
        return uuc_val_null();
    }
    return stack->head[stack->size - 1];
}

void stack_print(ValueStack *stack) {
    printf("Stack dump. Capacity: %d Size: %d. Content: [", stack->capacity, stack->size);
    for(int i = 0; i < stack->size; i++) {
        Value v = stack->head[i];
        uuc_val_print(v);
        printf(",");
    }
    printf("]\n");
}

void stack_free(Values *stack) {
    free(stack->head);
}
