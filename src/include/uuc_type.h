#ifndef uuc_type_h
#define uuc_type_h

#include <stdio.h>

typedef enum {
    TYPE_INT,
    TYPE_DOUBLE,
    TYPE_BOOL,
    TYPE_NULL,
} UucType;

typedef struct {
    UucType type;
    union {
        long uuc_int;
        double uuc_double;
        int uuc_bool;
    } as;
} Value;

static inline Value type_null() {
    return (Value){ .type = TYPE_NULL };
}

static inline Value type_int(int v) {
    return (Value){ .type = TYPE_INT, .as = { .uuc_int = v } };
}

const char *uuc_type_str(UucType type);

static inline void type_print(Value val) {
    switch(val.type) {
        case TYPE_INT: printf("%ld", val.as.uuc_int); break;
        case TYPE_DOUBLE: printf("%f", val.as.uuc_double); break;
        case TYPE_BOOL: printf("%s", val.as.uuc_bool ? "true" : "false" ); break;
        case TYPE_NULL: printf("null"); break;
        default: printf("unknown"); break;
    }
}

#endif
