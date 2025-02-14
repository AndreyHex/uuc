#include "../include/uuc_collection.h"
#include "../include/uuc_memory.h"
#include <stdint.h>

ValueList list_init(uint32_t initial_capacity) {
    if (initial_capacity < 0) {
        LOG_ERROR("List initial capacity cannot be less than 0.\n");
        exit(1);
    }
    Value *p = (Value *)malloc(initial_capacity * sizeof(Value));
    return (ValueList){.head = p, .capacity = initial_capacity, .size = 0};
}

Value list_get(ValueList *list, uint32_t index) {
    if (list->size <= index) {
        LOG_ERROR("Index out of bound! List size: %d. Index: %d\n", list->size,
                  index);
        exit(1);
    }
    return list->head[index];
}

uint64_t list_push(ValueList *list, Value val) {
    if (list->size == list->capacity) {
        size_t new_cap = INCREASE_CAPACITY(list->capacity);
        list->head = INCREASE_ARRAY(Value, list->head, list->capacity, new_cap);
        list->capacity = new_cap;
    }
    list->head[list->size] = val;
    list->size++;
#if defined(UUC_LOG_TRACE)
    list_print(list);
#endif
    return list->size - 1;
}

void list_print(ValueList *list) {
    printf("List dump. Capacity: %d Size: %d. Content: [", list->capacity,
           list->size);
    for (int i = 0; i < list->size; i++) {
        type_print(list->head[i]);
        printf(",");
    }
    printf("]\n");
}

void list_free(Values *list) { free(list->head); }
