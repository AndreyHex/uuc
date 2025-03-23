#ifndef uuc_type_print_h
#define uuc_type_print_h

#include "uuc_function.h"

static inline void uuc_val_print(Value val) {
    switch(val.type) {
        case TYPE_INT: printf("%ld", val.as.uuc_int); break;
        case TYPE_DOUBLE: printf("%f", val.as.uuc_double); break;
        case TYPE_BOOL: printf("%s", val.as.uuc_bool ? "true" : "false" ); break;
        case TYPE_NULL: printf("null"); break;
        case TYPE_OBJ: {
            switch(val.as.uuc_obj->type){
                case OBJ_STRING: {
                    UucString *str = (UucString*)val.as.uuc_obj;
                    printf("string:'%s'", str->content);
                    break;
                }
                case OBJ_FUNCTION: {
                    UucFunction *fn = (UucFunction*)val.as.uuc_obj;
                    printf("fn<%s>", fn->name->content);
                    break;
                }
                default: printf("obj"); break;
            }
            break;
        }
        default: printf("unknown"); break;
    }
}

#endif
