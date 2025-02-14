#ifndef uuc_type_h
#define uuc_type_h

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

typedef enum {
    OBJ_STRING,
} ObjType;

typedef enum {
    TYPE_INT,
    TYPE_DOUBLE,
    TYPE_BOOL,
    TYPE_NULL,
    TYPE_OBJ,
} UucType;

typedef struct {
    ObjType type;
} UucObj;

typedef struct {
    UucObj uuc_obj;
    uint64_t length;
    char content[];
} UucString;

typedef struct {
    UucType type;
    union {
        long uuc_int;
        double uuc_double;
        int uuc_bool;
        UucObj *uuc_obj;
    } as;
} Value;

static inline Value type_null() {
    return (Value){ .type = TYPE_NULL };
}

static inline Value type_int(int v) {
    return (Value){ .type = TYPE_INT, .as = { .uuc_int = v } };
}

static inline Value type_bool(int v) {
    return (Value){ .type = TYPE_BOOL, .as = { .uuc_bool = v } };
}

const char *uuc_type_str(UucType type);

static inline void type_print(Value val) {
    switch(val.type) {
        case TYPE_INT: printf("%ld", val.as.uuc_int); break;
        case TYPE_DOUBLE: printf("%f", val.as.uuc_double); break;
        case TYPE_BOOL: printf("%s", val.as.uuc_bool ? "true" : "false" ); break;
        case TYPE_NULL: printf("null"); break;
        case TYPE_OBJ: {
            if(val.as.uuc_obj->type == OBJ_STRING) {
                UucString *str = (UucString*)val.as.uuc_obj;
                printf("string:'%s'", str->content);
            } else printf("obj"); 
            break; 
        }
        default: printf("unknown"); break;
    }
}

#endif
