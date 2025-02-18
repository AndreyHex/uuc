#ifndef uuc_parser_h
#define uuc_parser_h

#include "uuc_lexer.h"
#include "uuc_bytecode.h"
#include "uuc_result.h"
#include <stdint.h>

typedef struct {
    LexerContext lexer_context;
    Slice bytecode;
    Token next_token;
    Token current_token;
    Token previous_token;
    int panic;
    int error;
} ParserContext;

UucResult parse_code(Slice *slice, char *code);

#endif
