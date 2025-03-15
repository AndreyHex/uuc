#ifndef uuc_parser_h
#define uuc_parser_h

#include "uuc_lexer.h"
#include "uuc_bytecode.h"
#include "uuc_result.h"
#include <stdint.h>

typedef struct {
    Token name;
    int depth;
} ParserLocal;

typedef struct {
    int depth;
    uint32_t index;
    uint32_t pos;
} ParserJump;

typedef struct {
    LexerContext lexer_context;
    Slice bytecode;
    Token current_token;
    Token previous_token;
    int panic;
    int error;
    ParserLocal locals[225]; // TODO make dynamic?
    uint32_t local_size;
    uint32_t scope_depth;
    uint32_t loop_depth;
    ParserJump break_jumps[225];
    uint32_t break_size;
    ParserJump continue_jumps[225];
    uint32_t continue_size;
} ParserContext;

UucResult parse_code(Slice *slice, char *code);

#endif
