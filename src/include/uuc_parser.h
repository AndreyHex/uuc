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
    LexerContext lexer_context;
    Slice bytecode;
    Token current_token;
    Token previous_token;
    int panic;
    int error;
    ParserLocal locals[225]; // TODO make dynamic?
    uint32_t local_size;
    uint32_t scope_depth;
} ParserContext;

UucResult parse_code(Slice *slice, char *code);

#endif
