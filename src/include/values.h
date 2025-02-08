#ifndef uuc_value_stack_h
#define uuc_value_stack_h

#include "log.h"
#include <stdint.h>
#include <stdlib.h>

typedef double Value;

typedef struct {
    Value *head;
    Value *tail;
    uint32_t capacity;
    uint32_t size;
} Values;

typedef Values ValueStack;
typedef Values ValueList;

ValueStack stack_init(uint32_t initial_capacity);
void stack_push(ValueStack *stack, Value val);
Value stack_pop(ValueStack *stack);
void stack_print(Values *stack);
void stack_free(Values *stack);

ValueList list_init(uint32_t initial_capacity);
uint64_t list_push(ValueList *list, Value val);
Value list_get(ValueList *list, uint32_t index);
void list_print(Values *list);
void list_free(Values *list);

#endif
