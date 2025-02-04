#ifndef lexer_h
#define lexer_h

#include "result.h"
#include "../include/token.h"

typedef struct {
    ResultType result;
    int size;
    TokenType type;
} TokenResult;

Tokens *scan(char *, int);
char peek(char *, int, int);
TokenResult parse_simple(char, char);
TokenResult parse_string(char *, int, int);
TokenResult parse_identifier(char *, int, int);
void add_token(Tokens *, Token);
void free_tokens(Tokens *);

#endif
