#ifndef uuc_precedence_h
#define uuc_precedence_h

#include "log.h"
#include "token.h"
#include <stdint.h>

uint8_t precedence(TokenType token_type) {
    switch(token_type) {
        case INTEGER:
        case DOUBLE: return 1;
        case PLUS: return 2;
        case MINUS: return 2;
        case STAR: return 4;
        case SLASH: return 4;
        case BANG: return 7;
        default: return 0;
    }
}

#endif
