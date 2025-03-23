#ifndef uuc_function_h
#define uuc_function_h

#include "uuc_token.h"
#include "uuc_type.h"
#include "uuc_bytecode.h"

typedef struct {
    UucObj uuc_obj;
    uint8_t arity;
    Slice bytecode;
    UucString *name;
} UucFunction;

UucFunction *uuc_create_function(char *name);
UucFunction *uuc_create_function_t(Token token);
void uuc_free_function(UucFunction *func);

#endif
