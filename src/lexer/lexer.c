#include "../include/lexer.h"
#include "../include/memory.h"
#include "../include/result.h"
#include "../include/log.h"
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
    {'>', GREATER, 1, {{'=', GREATER_EQUAL}}},
    {'<', LESS, 1, {{'=', LESS_EQUAL}}},
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

TokenResult lex_simple(LexerContext *ctx);
TokenResult lex_string(LexerContext *ctx);
TokenResult lex_number(LexerContext *ctx);
TokenResult lex_something(LexerContext *ctx, char *rest, TokenType candidate);
TokenResult lex_identifier(LexerContext *ctx, int parsed);

Token create_token(LexerContext *ctx, TokenType type, int size);
Token create_end(LexerContext *ctx);

char lexer_peek(LexerContext *ctx);
char lexer_peek_next(LexerContext *ctx);

int is_end(LexerContext *ctx);

int is_letter(char c);
int is_digit(char c);
int is_whitespace(char c);
int is_whitespace_no_new_line(char c);

LexerContext lexer_init_context(char *code) {
    return (LexerContext){ 
        .input = code,
        .cursor = 0,
        .in_comment = 0,
        .line = 0
    };
}

Token next_token(LexerContext *ctx) {
    char c = lexer_peek(ctx);

    if(is_end(ctx)) {
        return create_end(ctx);
    }

    while(is_whitespace(c) || ctx->in_comment) {
        if(is_end(ctx)) return create_end(ctx);
        if(c == '\n') {
            ctx->line++;
            if(ctx->in_comment) ctx->in_comment = 0;
        }
        ctx->cursor++;
        c = lexer_peek(ctx);
    }

    if (c == '"') {
        TokenResult res = lex_string(ctx);
        if(res.result == SOME) return create_token(ctx, res.type, res.size);
    }

    TokenResult simple_res = lex_simple(ctx);
    if (simple_res.result == SOME) {
        if(simple_res.type == SLASH_SLASH) ctx->in_comment = 1;
        return create_token(ctx, simple_res.type, simple_res.size);
    }

    if(is_digit(c)) {
        TokenResult nr = lex_number(ctx);
        if(nr.result == SOME) {
            return create_token(ctx, nr.type, nr.size);
        }
    }
    
    TokenResult rr = {0};
    switch (c) {
        case 'v': rr = lex_something(ctx, "var", VAR); break;
        case 't': { 
            char p = lexer_peek_next(ctx);
            switch(p) {
                case 'r': rr = lex_something(ctx, "true", TRUE); break;
                case 'h': rr = lex_something(ctx, "this", THIS); break;
                default: rr = lex_identifier(ctx, 0);
            }
            break; 
        }
        case 'f': {
            char p = lexer_peek_next(ctx);
            switch(p) {
                case 'n': rr = lex_something(ctx, "fn", FN); break;
                case 'a': rr = lex_something(ctx, "false", FALSE); break;
                case 'o': rr = lex_something(ctx, "for", FOR); break;
                default: rr = lex_identifier(ctx, 0);
            };
            break;
        }
        case 'i': rr = lex_something(ctx, "if", IF); break;
        case 'e': rr = lex_something(ctx, "else", ELSE); break;
        case 'w': rr = lex_something(ctx, "while", WHILE); break;
        case 'n': rr = lex_something(ctx, "null", TOKEN_NULL); break;
        case 's': rr = lex_something(ctx, "super", SUPER); break;
        case 'c': rr = lex_something(ctx, "class", CLASS); break;
        case 'r': rr = lex_something(ctx, "return", RETURN); break;
        default: rr = lex_identifier(ctx, 0);
    }
    if (rr.result == SOME) {
        return create_token(ctx, rr.type, rr.size);
    }

    return create_end(ctx);
}

Token create_end(LexerContext *ctx) {
    return create_token(ctx, TOKEN_EOF, 1);
}

Token create_token(LexerContext *ctx, TokenType type, int size) {
    Token t = {
        .start = &ctx->input[ctx->cursor - size], // begin pointer + current position - token size
        .line = ctx->line,
        .type = type,
        .pos = ctx->cursor - size,
        .length = size
    };
    LOG_TRACE("Scanned token '%s' at %d:%d length %d\n", token_name(type), t.line, t.pos, t.length);
    return t;
}

Tokens *scan(char *input, int size) {
    LOG_TRACE("Scanning input string '%s'\n", input);
    Tokens *tokens = allocate(sizeof(Tokens));
    tokens->tokens = ALLOC_ARRAY(Token);
    tokens->capacity = 16;
    tokens->size = 0;
    
    LexerContext ctx = { 
        .input = input, 
        .line = 0, 
        .cursor = 0, 
        .in_comment = 0 
    };

    while(1) {
        Token next = next_token(&ctx);
        add_token(tokens, next);
        if(next.type == TOKEN_EOF) {
            break;
        }
    }
    return tokens;
}

char lexer_peek(LexerContext *ctx) {
    return ctx->input[ctx->cursor];
}

char lexer_peek_next(LexerContext *ctx) {
    if(is_end(ctx)) return '\0';
    return ctx->input[ctx->cursor + 1];
}

int is_end(LexerContext *ctx) {
    return lexer_peek(ctx) == '\0' || lexer_peek(ctx) == EOF;
}

TokenResult lex_string(LexerContext *ctx) {
    TokenResult r = {.result = SOME, .size = 1, .type = STRING}; // initial 1 for open "
    char c = lexer_peek(ctx);
    if(c != '"') {
        LOG_ERROR("BIG ERRORR\n");
        exit(1);
    }
    ctx->cursor++;
    c = lexer_peek(ctx);
    char p = '"';
    while ((c != '"' || p == '\\') && c != '\0' && c != EOF) {
        ctx->cursor++;
        r.size++;
        p = c;
        c = lexer_peek(ctx);
    }
    if(c == '\0' || c == EOF) {
        LOG_ERROR("Unexpected end of input.\n");
        r.type = TOKEN_ERROR;
        return r;
    }
    ctx->cursor++; // closing "
    r.size++;      // same
    return r;
}

TokenResult lex_number(LexerContext *ctx) {
    char c = lexer_peek(ctx);
    int size = 0;
    TokenType t = INTEGER;
    while(is_digit(c) || (c == '.' && is_digit(lexer_peek_next(ctx)))) {
        if(c == '.') t = DOUBLE;
        size++;
        ctx->cursor++;
        c = lexer_peek(ctx);
    }
    TokenResult r = { .result = SOME, .size = size, .type = t };
    return r;
}

TokenResult lex_something(LexerContext *ctx, char *name, TokenType candidate) {
    int i = 0;
    char r = name[i];
    while(r != '\0') {
        if(r != lexer_peek(ctx) || is_end(ctx)) return lex_identifier(ctx, i);
        i ++;
        r = name[i];
        ctx->cursor++;
    }
    if(r != '\0') return lex_identifier(ctx, i);
    char next_after = lexer_peek(ctx);
    // check if next is valid identifier character
    if(is_digit(next_after) || is_letter(next_after) || next_after == '_') {
        return lex_identifier(ctx, i);
    }
    TokenResult rr = { .result = SOME, .size = i, .type = candidate };
    return rr;
}

TokenResult lex_identifier(LexerContext *ctx, int parsed) {
    TokenResult r = { .result = SOME, .size = parsed, .type = IDENTIFIER};
    int i = parsed;
    char c = lexer_peek(ctx);
    while (c == '_' || is_letter(c) || is_digit(c)) {
        i++;
        ctx->cursor++;
        c = lexer_peek(ctx);
    }
    r.size = i;
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

TokenResult lex_simple(LexerContext *ctx) {
    char c = lexer_peek(ctx);
    char n = lexer_peek_next(ctx);
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
                    ctx->cursor += 2;
                    return r;
                }
            }
            r.type = target.type;
            r.size = 1;
            r.result = SOME;
            ctx->cursor += 1;
            return r;
        }
    }
    return r;
}

void add_token(Tokens *tokens, Token token) {
    if (tokens->size == tokens->capacity - 1) {
        int new_capacity = INCREASE_CAPACITY(tokens->capacity);
        tokens->tokens = INCREASE_ARRAY(Token, tokens->tokens, tokens->capacity,
                                        new_capacity);
        tokens->capacity = new_capacity;
    }
    tokens->tokens[tokens->size] = token;
    tokens->size++;
}

void free_tokens(Tokens *tokens) {
    Token *types = tokens->tokens;
    int capacity = tokens->capacity;
    FREE_ARRAY(Token, types, capacity);
    free(tokens);
}
