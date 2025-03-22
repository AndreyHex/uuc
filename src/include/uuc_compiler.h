#ifndef uuc_compiler_h
#define uuc_compiler_h

#include "uuc_function.h"
#include "uuc_lexer.h"
#include "uuc_bytecode.h"
#include "uuc_result.h"
#include <stdint.h>

typedef struct {
    Token name;
    int depth;
} CompilerLocal;

typedef struct {
    int depth;
    uint32_t index;
    uint32_t pos;
} CompilerJump;

typedef struct {
    LexerContext lexer_context;
    UucFunction *main;
    UucFunction *current;
    Token current_token;
    Token previous_token;
    int panic;
    int error;
    CompilerLocal locals[225]; // TODO make dynamic?
    uint32_t local_size;
    uint32_t scope_depth;
    int loop_depth;

    CompilerJump break_jumps[225];
    uint32_t break_size;
    CompilerJump continue_jumps[225];
    uint32_t continue_size;
} CompilerContext;

UucResult compile_code(UucFunction *main, char *code);

static inline Slice *uuc_cur_bcode(CompilerContext *context) {
    return &context->current->bytecode;
}

#endif
