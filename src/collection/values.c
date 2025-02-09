#include "../include/collection.h"
#include <string.h>

Values collection_copy(Values collection) {
    int cap = collection.capacity;
    int size = collection.size;
    Value *ptr = malloc(cap * sizeof(Value));
    memcpy(ptr, collection.head, cap);
    return (Values){
        .head = ptr,
        .capacity = cap,
        .size = size,
        .tail = &ptr[size - 1],
    };
}
