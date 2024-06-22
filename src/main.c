#include "lexer.h"
#include <stdio.h>

int main(int argc, const char *argv[]) {

    char *code =
        "+---++/-+/*/-*!!\n\n!+==!=!====--\n++=/-*/-+/-/+/-++-/-/-*/*-/+*-/-/";
    Tokens *t = scan(code, 44);
    for (int i = 0; i < t->count; i++) {
        Token token = t->tokens[i];
        printf("%d:%d '", token.line, token.pos);
        for (int k = 0; k < token.length; k++)
            printf("%c", token.start[k]);
        printf("' code:%d\n", token.type);
    }

    return 0;
}
