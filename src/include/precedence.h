#ifndef uuc_precedence_h
#define uuc_precedence_h

#include "log.h"
#include "token.h"
#include <stdint.h>

typedef enum {
    PREC_COMPARISON, // < <= > >=
    PREC_TERM,       // - +
    PREC_FACTOR,     // * /
    PREC_UNARY,      // ! -
    PREC_CALL,       // . ()
} Precedence;

typedef struct {
    uint8_t l;
    uint8_t r;
} Binding;

Binding binding_power(TokenType token_type) {
    switch(token_type) {
        case PLUS: return {1, 2};
        case MINUS: return {1, 2};
        case STAR: return {3, 4};
        case SLASH: return {3, 4};
        default: {
            LOG_ERROR("Getting binding power for unsupported token_type '%s'. Returning {0,0}.\n", token_name(token_type));
            return {0};
        }
    }
}

#endif
