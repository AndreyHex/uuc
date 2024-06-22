#ifndef lexer_h
#define lexer_h

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
    AND_B,
    OR,
    OR_B,

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

    EOF,
} TokenType;

typedef struct {
    TokenType type;
    const char *start;
    int length;
    int line;
    int pos;
} Token;

typedef struct {
    int count;
    int capacity;
    Token *tokens;
} Tokens;

Tokens *scan(char *, int);
char peek(char *, int, int);
void add_token(Tokens *, Token);

#endif
