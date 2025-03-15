#ifndef uuc_token_h
#define uuc_token_h

typedef enum {
    TOKEN_LPAREN,
    TOKEN_RPAREN,
    TOKEN_LBRACE,
    TOKEN_RBRACE,
    TOKEN_COMMA,
    TOKEN_DOT,
    TOKEN_SEMICOLON,
    TOKEN_COLON,

    TOKEN_PLUS,
    TOKEN_PLUS_PLUS,
    TOKEN_PLUS_EQUAL,
    TOKEN_MINUS,
    TOKEN_MINUS_MINUS,
    TOKEN_MINUS_EQUAL,
    TOKEN_STAR,
    TOKEN_STAR_EQUAL,
    TOKEN_SLASH,
    TOKEN_SLASH_SLASH,
    TOKEN_SLASH_EQUAL,

    TOKEN_BANG,
    TOKEN_BANG_EQUAL,
    TOKEN_EQUAL,
    TOKEN_EQUAL_EQUAL,
    TOKEN_LESS,
    TOKEN_LESS_EQUAL,
    TOKEN_GREATER,
    TOKEN_GREATER_EQUAL,

    TOKEN_IDENTIFIER,
    TOKEN_STRING,
    TOKEN_INTEGER,
    TOKEN_DOUBLE,
    TOKEN_TRUE,
    TOKEN_FALSE,

    TOKEN_AND,
    TOKEN_AND_AND,
    TOKEN_AND_EQUAL,
    TOKEN_OR,
    TOKEN_OR_OR,
    TOKEN_OR_EQUAL,

    TOKEN_CLASS,
    TOKEN_FN,
    TOKEN_RETURN,
    TOKEN_VAR,
    TOKEN_THIS,
    TOKEN_SUPER,

    TOKEN_BREAK,
    TOKEN_CONTINUE,

    TOKEN_FOR,
    TOKEN_WHILE,
    TOKEN_IF,
    TOKEN_ELSE,

    TOKEN_NULL,

    TOKEN_EOF,
    TOKEN_ERROR,
} TokenType;

typedef struct {
    TokenType type;
    const char *start;
    int length;
    int line;
    int pos;
} Token;

typedef struct {
    int size;
    int capacity;
    Token *tokens;
} Tokens;

const char* token_name(TokenType token_type);

#endif
