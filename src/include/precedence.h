#ifndef uuc_precedence_h
#define uuc_precedence_h

#include "log.h"
#include "token.h"
#include <stdint.h>

uint8_t precedence(TokenType token_type) {
    switch(token_type) {
        case TOKEN_EOF:
        case SEMICOLON: return 0;
        case NUMBER: return 1;
        case PLUS: return 2;
        case MINUS: return 3;
        case STAR: return 4;
        case SLASH: return 5;
        default: return 0;
    }
}

#endif
