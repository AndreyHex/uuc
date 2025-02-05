#include "../include/token.h"

int panic = 0;
int error = 0;

int cursor = 0;

Tokens tokens;

Token next() {
    return tokens.tokens[cursor++];
}

Token peek() {
    return tokens.tokens[cursor + 1];
}

void parse(Tokens tokens) {
}
