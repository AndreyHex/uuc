#ifndef uuc_parser_h
#define uuc_parser_h

#include "uuc_lexer.h"
#include "uuc_bytecode.h"
#include <stdint.h>

typedef struct {
    LexerContext lexer_context;
    Slice bytecode;
    Token next_token;
    Token current_token;
    Token previous_token;
} ParserContext;

Slice parse_code(char *code);

#endif
