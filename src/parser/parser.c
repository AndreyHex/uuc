#include "../include/token.h"
#include "../include/parser.h"
#include "../include/lexer.h"

int panic = 0;
int error = 0;

int cursor = 0;

Tokens tokens;

ParserContext context;

Token next() {
    return tokens.tokens[cursor++];
}

Token peek() {
    return tokens.tokens[cursor + 1];
}

void parse_code(char *code) {
    LexerContext lexer_context = lexer_init_context(code);
    context = (ParserContext){ .lexer_context  = lexer_context };

    Token next = next_token(&context.lexer_context);


}
