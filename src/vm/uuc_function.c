#include "../include/uuc_function.h"
#include "../include/uuc_string.h"

UucFunction *uuc_create_function(char *name) {
    UucString *fn_name = uuc_create_string(name);
    UucFunction *fn = calloc(1, sizeof(UucFunction));
    fn->name = fn_name;
    fn->uuc_obj = (UucObj){ .type = OBJ_FUNCTION };
    fn->bytecode = slice_init(8);
    return fn;
}

UucFunction *uuc_create_function_t(Token token) {
    UucString *fn_name = uuc_copy_string(token.start, token.length);
    UucFunction *fn = calloc(1, sizeof(UucFunction));
    fn->name = fn_name;
    fn->uuc_obj = (UucObj){ .type = OBJ_FUNCTION };
    fn->bytecode = slice_init(8);
    return fn;
}

void uuc_free_function(UucFunction *func) {
    uuc_slice_free(&func->bytecode);
    uuc_free_string(func->name);
    free(func);
}

