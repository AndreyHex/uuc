#include "lexer.h"
#include "memory.h"

Tokens *scan(char *input, int size) {
    int i = 0;
    int line = 0;

    Tokens *tokens = allocate(sizeof(Tokens));
    tokens->tokens = ALLOC_ARRAY(Token);
    tokens->capacity = 16;
    tokens->count = 0;

    while (i <= size) {
        char c = input[i];
        Token token;
        token.pos = i;
        token.start = &input[i];
        token.line = line;
        int l = 1;
        switch (c) {
            case '+':
                {
                    char p = peek(input, size, i);
                    if (p == '+') {
                        token.type = PLUS_PLUS;
                        i++;
                        l++;
                    } else token.type = PLUS;
                    break;
                }
            case '-':
                token.type = MINUS;
                break;
                {
                    char p = peek(input, size, i);
                    if (p == '-') {
                        token.type = MINUS_MINUS;
                        i++;
                        l++;
                    } else token.type = MINUS;
                    break;
                }
            case '*': token.type = STAR; break;
            case '/': token.type = SLASH; break;
            case '=':
                {
                    char p = peek(input, size, i);
                    if (p == '=') {
                        token.type = EQUAL_EQUAL;
                        i++;
                        l++;
                    } else token.type = EQUAL;
                    break;
                }
            case '!':
                {
                    char p = peek(input, size, i);
                    if (p == '=') {
                        token.type = BANG_EQUAL;
                        i++;
                        l++;
                    } else token.type = BANG;
                    break;
                }
            case '\n':
                {
                    line++;
                    i++;
                    continue;
                }
            default:
                {
                    i++;
                    continue;
                }
        }
        token.length = l;

        add_token(tokens, token);

        i++;
    }
    return tokens;
}

char peek(char *input, int size, int i) {
    if (i + 1 >= size) return EOF;
    return input[i + 1];
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
