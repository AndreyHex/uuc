#ifndef uuc_type_h
#define uuc_type_h

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

#endif
