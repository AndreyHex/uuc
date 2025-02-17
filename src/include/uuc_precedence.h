#ifndef uuc_precedence_h
#define uuc_precedence_h

#include "uuc_log.h"
#include "uuc_token.h"
#include <stdint.h>

uint8_t precedence(TokenType token_type) {
    switch(token_type) {
        case TOKEN_EQUAL:
        case TOKEN_INTEGER:
        case TOKEN_DOUBLE: return 2;
        case TOKEN_EQUAL_EQUAL: 
        case TOKEN_BANG_EQUAL: return 3;
        case TOKEN_LESS:
        case TOKEN_LESS_EQUAL:
        case TOKEN_GREATER:
        case TOKEN_GREATER_EQUAL: return 4;
        case TOKEN_PLUS: return 5;
        case TOKEN_MINUS: return 5;
        case TOKEN_STAR: return 6;
        case TOKEN_SLASH: return 6;
        case TOKEN_BANG: return 8;
        default: return 0;
    }
}

#endif
