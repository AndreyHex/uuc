#ifndef uuc_precedence_h
#define uuc_precedence_h

#include "log.h"
#include "token.h"
#include <stdint.h>

uint8_t precedence(TokenType token_type) {
    switch(token_type) {
        case INTEGER:
        case DOUBLE: return 1;
        case EQUAL_EQUAL: 
        case BANG_EQUAL: return 3;
        case LESS:
        case LESS_EQUAL:
        case GREATER:
        case GREATER_EQUAL: return 4;
        case PLUS: return 5;
        case MINUS: return 5;
        case STAR: return 6;
        case SLASH: return 6;
        case BANG: return 8;
        default: return 0;
    }
}

#endif
