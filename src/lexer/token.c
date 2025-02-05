#include "../include/token.h"

const char *TOKEN_NAMES[] = {
    "LEFT_PAREN",
    "RIGHT_PAREN",
    "LEFT_BRACE",
    "RIGHT_BRACE",
    "COMMA",
    "DOT",
    "SEMICOLON",
    "COLON",

    "PLUS",
    "PLUS_PLUS",
    "PLUS_EQUAL",
    "MINUS",
    "MINUS_MINUS",
    "MINUS_EQUAL",
    "STAR",
    "STAR_EQUAL",
    "SLASH",
    "SLASH_SLASH",
    "SLASH_EQUAL",

    "BANG",
    "BANG_EQUAL",
    "EQUAL",
    "EQUAL_EQUAL",
    "LESS",
    "LESS_EQUAL",
    "GREATER",
    "GREATER_EQUAL",

    "IDENTIFIER",
    "STRING",
    "NUMBER",
    "TRUE",
    "FALSE",

    "AND",
    "AND_AND",
    "AND_EQUAL",
    "OR",
    "OR_OR",
    "OR_EQUAL",

    "CLASS",
    "FN",
    "RETURN",
    "VAR",
    "THIS",
    "SUPER",

    "FOR",     
    "WHILE",
    "IF",
    "ELSE",

    "UUC_NULL",

    "TOKEN_EOF",
    "TOKEN_ERROR"
};

const char *token_name(TokenType token_type) { return TOKEN_NAMES[token_type]; }
