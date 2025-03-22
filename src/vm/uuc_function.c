#include "../include/uuc_function.h"
#include "../include/uuc_string.h"

UucFunction uuc_create_function(char *name) {
    UucString *fn_name = uuc_create_string(name);
    return (UucFunction){
        .name = fn_name,
        .arity = 0,
        .uuc_obj = (UucObj){ .type = OBJ_FUNCTION },
        .bytecode = slice_init(8),
    };
}

void uuc_free_function(UucFunction *func) {
    uuc_slice_free(&func->bytecode);
    uuc_free_string(func->name);
}

