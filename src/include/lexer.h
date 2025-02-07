#ifndef lexer_h
#define lexer_h

#include "result.h"
#include "../include/token.h"

typedef struct {
    char *input;
    int cursor;
    int line;
    int in_comment;
} LexerContext;

typedef struct {
    ResultType result;
    int size;
    TokenType type;
} TokenResult;

LexerContext lexer_init_context(char *code);
Tokens *scan(char *, int);
Token next_token(LexerContext *ctx);

void add_token(Tokens *, Token);
void free_tokens(Tokens *);

#endif
