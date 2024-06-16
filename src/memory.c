#include "memory.h"
#include <stdio.h>
#include <stdlib.h>

void *reallocate(void *ptr, size_t old_size, size_t new_size) {
    if (new_size == 0) {
        free(ptr);
        return NULL;
    }
    printf("hello");
    void *res = realloc(ptr, new_size);
    if (res == NULL) {
        printf("Unable to reallocate memory.");
        exit(1);
    }
    return res;
}
