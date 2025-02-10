#include "../include/collection.h"
#include "../include/memory.h"
#include <stdint.h>

ValueList list_init(uint32_t initial_capacity) {
    if(initial_capacity < 0) {
        LOG_ERROR("List initial capacity cannot be less than 0.\n");
        exit(1);
    }
    Value *p = (Value*) malloc(initial_capacity * sizeof(Value));
    return (ValueList){
        .head = p,
        .tail = NULL,
        .capacity = initial_capacity,
        .size = 0
    };
}

Value list_get(ValueList *list, uint32_t index) {
    if(list->size <= index) {
        LOG_ERROR("Index out of bound! List size: %d. Index: %d\n", list->size, index);
        exit(1);
    }
    return list->head[index];
}

uint64_t list_push(ValueList *list, Value val) {
    #if defined (UUC_LEVEL_TRACE)
    list_print(list);
    #endif
    if(list->size == list->capacity) {
        uint64_t new_cap = INCREASE_CAPACITY(list->capacity);
        LOG_TRACE("Growing list size from %d to %ld\n", list->size, new_cap);
        list->head = INCREASE_ARRAY(Value, list->head, list->capacity, new_cap);
        list->capacity = new_cap;
    }
    list->head[list->size] = val;
    list->tail = &list->head[list->size];
    list->size++;
    return list->size - 1;
}

void list_print(ValueList *list) {
    printf("List dump. Capacity: %d Size: %d. Content: [", list->capacity, list->size);
    for(int i = 0; i < list->size; i++) {
        printf("%f, ", list->head[i]);
    }
    printf("]\n");
}

void list_free(Values *list) {
    free(list->head);
}

