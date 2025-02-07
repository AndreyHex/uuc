#ifndef uuc_token_h
#define uuc_token_h

typedef enum {
    LEFT_PAREN,
    RIGHT_PAREN,
    LEFT_BRACE,
    RIGHT_BRACE,
    COMMA,
    DOT,
    SEMICOLON,
    COLON,

    PLUS,
    PLUS_PLUS,
    PLUS_EQUAL,
    MINUS,
    MINUS_MINUS,
    MINUS_EQUAL,
    STAR,
    STAR_EQUAL,
    SLASH,
    SLASH_SLASH,
    SLASH_EQUAL,

    BANG,
    BANG_EQUAL,
    EQUAL,
    EQUAL_EQUAL,
    LESS,
    LESS_EQUAL,
    GREATER,
    GREATER_EQUAL,

    IDENTIFIER,
    STRING,
    NUMBER,
    TRUE,
    FALSE,

    AND,
    AND_AND,
    AND_EQUAL,
    OR,
    OR_OR,
    OR_EQUAL,

    CLASS,
    FN,
    RETURN,
    VAR,
    THIS,
    SUPER,

    FOR,
    WHILE,
    IF,
    ELSE,

    UUC_NULL,

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
