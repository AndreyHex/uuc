#include "../include/lexer.h"
#include "../include/memory.h"
#include "../include/result.h"
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

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

TokenResult parse_number(char *input, int size, int index);
TokenResult parse_something(char *input, int size, int index, char *rest, TokenType candidate);
TokenResult parse_identifier(char *input, int size, int index);

int is_letter(char c);
int is_digit(char c);
int is_whitespace(char c);
int is_whitespace_no_new_line(char c);

Tokens *scan(char *input, int size) {
    int i = 0;
    int line = 0;
    int comment = 0;

    Tokens *tokens = allocate(sizeof(Tokens));
    tokens->tokens = ALLOC_ARRAY(Token);
    tokens->capacity = 16;
    tokens->count = 0;

    while (i < size) {
        char c = input[i];
        if (is_whitespace_no_new_line(c)) {
            i++;
            continue;
        }

        if (comment) {
            i++;
            if (c == '\n') {
                comment = 0;
                line++;
            }
            continue;
        }
        if (c == '\n') {
            i++;
            line++;
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

        if(is_digit(c)) {
            printf("parsing number\n");
            TokenResult nr = parse_number(input, size, i);
            if(nr.result == SOME) {
                token.type = nr.type;
                add_token(tokens, token);
                i += nr.size;
                continue;
            }
        }

        TokenResult rr = {0};
        switch (c) {
            case 'v': rr = parse_something(input, size, i, "var", VAR); break;
            case 't': { 
                char p = peek(input, size, i);
                switch(p) {
                    case 'r': rr = parse_something(input, size, i, "true", TRUE); break;
                    case 'h': rr = parse_something(input, size, i, "this", THIS); break;
                    default: rr = parse_identifier(input, size, i);
                }
                break; 
            }
            case 'f': {
                char p = peek(input, size, i);
                switch(p) {
                    case 'n': rr = parse_something(input, size, i, "fn", FN); break;
                    case 'a': rr = parse_something(input, size, i, "false", FALSE); break;
                    case 'o': rr = parse_something(input, size, i, "for", FOR); break;
                    default: rr = parse_identifier(input, size, i);
                };
                break;
            }
            case 'i': rr = parse_something(input, size, i, "if", IF); break;
            case 'e': rr = parse_something(input, size, i, "else", ELSE); break;
            case 'w': rr = parse_something(input, size, i, "while", WHILE); break;
            case 'n': rr = parse_something(input, size, i, "null", UUC_NULL); break;
            case 's': rr = parse_something(input, size, i, "super", SUPER); break;
            case 'c': rr = parse_something(input, size, i, "class", CLASS); break;
            case 'r': rr = parse_something(input, size, i, "return", RETURN); break;
            default: rr = parse_identifier(input, size, i);
        }
        if (rr.result == SOME) {
            token.type = rr.type;
            add_token(tokens, token);
            i += rr.size;
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
    TokenResult r = {.result = SOME, .size = 0, .type = STRING};
    char p = '"';
    char c = input[index];
    while ((c != '"' || p == '\\') && c != '\0' && c != EOF) {
        r.size++;
        p = c;
        c = input[index + r.size];
    }
    return r;
}

TokenResult parse_number(char *input, int size, int index) {
    int i = 0;
    while(index + i < size && is_digit(input[index + i])) i++;
    if(index + i < size && input[index + i] == '.' && is_digit(peek(input, size, index + i))) {
        i++; // skip dot
        while(index + i < size && is_digit(input[index + i])) i++;
    }
    TokenResult r = { SOME, i, NUMBER };
    return r;
}

TokenResult parse_something(char *input, int size, int index, char *name, TokenType candidate) {
    int i = 0;
    char r = name[i];
    while( index + i < size && r != '\0' ) {
        if( r != input[index + i] ) return parse_identifier(input, size, index);
        i ++;
        r = name[i];
    }
    if( r != '\0' ) return parse_identifier(input, size, index);
    char next_after = peek(input, size, index + i - 1);
    // check if next is valid identifier character
    if(is_digit(next_after) || is_letter(next_after) || next_after == '_') {
        return parse_identifier(input, size, index);
    }
    TokenResult rr = { SOME, i, candidate };
    return rr;
}

TokenResult parse_identifier(char *input, int size, int index) {
    TokenResult r = {NONE, 0, IDENTIFIER};
    int i = index;
    char c = input[index];
    if (c == '_' || is_letter(c) || is_digit(c)) {
        r.result = SOME;
        i++;
        c = input[i];
    }
    while ((is_letter(c) || is_digit(c) || c == '_') && i < size) {
        i++;
        c = input[i];
    }
    r.size = i - index;
    return r;
}

int is_letter(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

int is_digit(char c) { return c >= '0' && c <= '9'; }

int is_whitespace(char c) {
    return c == ' ' || c == '\n' || c == '\t' || c == '\r' || c == '\0' ||
           c == EOF;
}

int is_whitespace_no_new_line(char c) {
    return c == ' '|| c == '\t' || c == '\r' || c == '\0' || c == EOF;
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
