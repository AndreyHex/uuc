#ifndef memory_h
#define memory_h 

#include <stdio.h>

#define INCREASE_CAPACITY(capacity) ((capacity) < 16 ? 16 : (capacity) * 2)
#define ALLOC_ARRAY(type) (type*)allocate(sizeof(type) * 16)
#define INCREASE_ARRAY(type, ptr, old_capacity, new_capacity)                  \
    (type *)reallocate(ptr, sizeof(type) * (old_capacity),                     \
                       sizeof(type) * (new_capacity))
#define FREE_ARRAY(type, ptr, old_capacity)                                    \
    reallocate(ptr, sizeof(type) * (old_capacity), 0)

void* reallocate(void* pointer, size_t old_size, size_t new_size);
void* allocate(size_t size); 

#endif
