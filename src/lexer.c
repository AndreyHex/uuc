#include "lexer.h"
#include "memory.h"
#include "result.h"
#include <stdio.h>
#include <stdlib.h>

typedef struct {
    char character;
    TokenType type;
} Pair;

typedef struct {
    char character;
    TokenType type;
    int p_size;
    Pair pairs[2];
} Rule;

Rule rules[] = {
    {'+', PLUS, 2, {{'+', PLUS_PLUS}, {'=', PLUS_EQUAL}}},
    {'-', MINUS, 2, {{'-', MINUS_MINUS}, {'=', MINUS_EQUAL}}},
    {'=', EQUAL, 1, {{'=', EQUAL_EQUAL}}},
    {'!', BANG, 1, {{'=', BANG_EQUAL}}},
    {'&', AND, 2, {{'&', AND_AND}, {'=', AND_EQUAL}}},
    {'|', OR, 2, {{'|', OR_OR}, {'=', OR_EQUAL}}},
    {'*', STAR, 1, {{'=', STAR_EQUAL}}},
    {'/', SLASH, 2, {{'/', SLASH_SLASH}, {'=', SLASH_EQUAL}}},
    {':', COLON, 0, {}},
    {';', SEMICOLON, 0, {}},
    {'.', DOT, 0, {}},
    {',', COMMA, 0, {}},
    {'(', LEFT_PAREN, 0, {}},
    {')', RIGHT_PAREN, 0, {}},
    {'{', LEFT_BRACE, 0, {}},
    {'}', RIGHT_BRACE, 0, {}},
};

int rules_size = sizeof(rules) / sizeof(Rule);

Tokens *scan(char *input, int size) {
    int i = 0;
    int line = 0;
    int comment = 0;

    Tokens *tokens = allocate(sizeof(Tokens));
    tokens->tokens = ALLOC_ARRAY(Token);
    tokens->capacity = 16;
    tokens->count = 0;

    while (i <= size) {
        char c = input[i];

        if (c == '\n') line++;

        if (comment) {
            i++;
            if (c == '\n') comment = 0;
            continue;
        }

        char n = peek(input, size, i);
        Token token;
        token.pos = i;
        token.start = &input[i];
        token.line = line;
        int l = 1;
        token.length = l;

        if (c == '"') {
            i++; // skipping open "
            TokenResult r = parse_string(input, size, i);
            token.type = r.type;
            add_token(tokens, token);
            i += r.size;
            i++; // skipping closing "
            continue;
        }

        TokenResult r = parse_simple(c, n);
        if (r.result == SOME) {
            token.type = r.type;
            add_token(tokens, token);
            if (r.type == SLASH_SLASH) comment = 1;
            i += r.size;
            continue;
        } 

        TokenResult identifier = parse_identifier(input, size, i);
        if(identifier.result == SOME) {
            token.type = identifier.type;
            add_token(tokens, token);
            i+= r.size;
            continue;
        }

        i++;
    }
    return tokens;
}

char peek(char *input, int size, int i) {
    if ((1 + i) >= size) return EOF;
    return input[i + 1];
}

TokenResult parse_string(char *input, int size, int index) {
    TokenResult r;
    r.result = SOME;
    r.size = 0;
    r.type = STRING;
    char p = '"';
    char c = input[index];
    while ((c != '"' || p == '\\') && c != '\0' && c != EOF) {
        r.size++;
        p = c;
        c = input[index + r.size];
    }
    return r;
}

TokenResult parse_identifier(char *input, int size, int index) {
    TokenResult r;
    r.result = NONE;
    r.size = 0;

    char c = input[index];
    if(c == 'v') {
    }


    return r;
}

TokenResult parse_simple(char c, char n) {
    TokenResult r;
    r.result = NONE;
    r.size = 0;
    for (int i = 0; i < rules_size; i++) {
        if (rules[i].character == c) {
            Rule target = rules[i];
            for (int k = 0; k < target.p_size; k++) {
                if (target.pairs[k].character == n) {
                    Pair p = target.pairs[k];
                    r.type = p.type;
                    r.size = 2;
                    r.result = SOME;
                    return r;
                }
            }
            r.type = target.type;
            r.size = 1;
            r.result = SOME;
            return r;
        }
    }
    return r;
}

void add_token(Tokens *tokens, Token token) {
    if (tokens->count == tokens->capacity - 1) {
        int new_capacity = INCREASE_CAPACITY(tokens->capacity);
        tokens->tokens = INCREASE_ARRAY(Token, tokens->tokens, tokens->capacity,
                                        new_capacity);
        tokens->capacity = new_capacity;
    }
    tokens->tokens[tokens->count] = token;
    tokens->count++;
}

void free_tokens(Tokens *tokens) {
    Token *types = tokens->tokens;
    int capacity = tokens->capacity;
    FREE_ARRAY(Token, types, capacity);
    free(tokens);
}
