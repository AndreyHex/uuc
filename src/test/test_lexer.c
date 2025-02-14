#include "../include/lexer.h"
#include "uuc_test.h"
#include "test_lexer.h"
#include <string.h>
#include <stdio.h>

typedef struct {
    char *input;
    TokenType expected[100];
    int exp_size;
} TestCase;

TestCase test_cases[] = {
    {"++--", {TOKEN_PLUS_PLUS, TOKEN_MINUS_MINUS}, 2},
    {"+-", {TOKEN_PLUS, TOKEN_MINUS}, 2},
    {"(())", {TOKEN_LPAREN, TOKEN_LPAREN, TOKEN_RPAREN, TOKEN_RPAREN}, 4},
    {"({})", {TOKEN_LPAREN, TOKEN_LBRACE, TOKEN_RBRACE, TOKEN_RPAREN}, 4},
    {"&&&", {TOKEN_AND_AND, TOKEN_AND}, 2},
    {"&&=&", {TOKEN_AND_AND, TOKEN_EQUAL, TOKEN_AND}, 3},
    {"&&=&=", {TOKEN_AND_AND, TOKEN_EQUAL, TOKEN_AND_EQUAL}, 3},
    {"..+-/=", {TOKEN_DOT, TOKEN_DOT, TOKEN_PLUS, TOKEN_MINUS, TOKEN_SLASH_EQUAL}, 5},
    {"..//+-/=\n+", {TOKEN_DOT, TOKEN_DOT, TOKEN_SLASH_SLASH, TOKEN_PLUS}, 4},
    {"\"string\"", {TOKEN_STRING}, 1},
    {"\"st\\\"ring\"", {TOKEN_STRING}, 1},
    {"\"st\\\"ring\"+\"!\"+\".\"", {TOKEN_STRING, TOKEN_PLUS, TOKEN_STRING, TOKEN_PLUS, TOKEN_STRING}, 5},
    {"(", {TOKEN_LPAREN}, 1},

    // var and identifiers
    {"var identifier =", {TOKEN_VAR, TOKEN_IDENTIFIER, TOKEN_EQUAL}, 3},
    {"identifier + identifir;", {TOKEN_IDENTIFIER, TOKEN_PLUS, TOKEN_IDENTIFIER, TOKEN_SEMICOLON}, 4},
    {"var ident111ifier", {TOKEN_VAR, TOKEN_IDENTIFIER}, 2},
    {"var _", {TOKEN_VAR, TOKEN_IDENTIFIER}, 2},
    {"var _123", {TOKEN_VAR, TOKEN_IDENTIFIER}, 2},
    {"var _1dSASSds_ds23", {TOKEN_VAR, TOKEN_IDENTIFIER}, 2},
    {"var __1_23", {TOKEN_VAR, TOKEN_IDENTIFIER}, 2},
    {"var_123 _", {TOKEN_IDENTIFIER, TOKEN_IDENTIFIER}, 2},
    // keyword is cut
    {"var tru", {TOKEN_VAR, TOKEN_IDENTIFIER}, 2},
    {"var tru\ne", {TOKEN_VAR, TOKEN_IDENTIFIER, TOKEN_IDENTIFIER}, 3},
    // true
    {"var _123 = true", {TOKEN_VAR, TOKEN_IDENTIFIER, TOKEN_EQUAL, TOKEN_TRUE}, 4},
    {"!false!=true", {TOKEN_BANG, TOKEN_FALSE, TOKEN_BANG_EQUAL, TOKEN_TRUE}, 4},
    {"!true!=false;", {TOKEN_BANG, TOKEN_TRUE, TOKEN_BANG_EQUAL, TOKEN_FALSE, TOKEN_SEMICOLON}, 5},
    {"true>true", {TOKEN_TRUE, TOKEN_GREATER, TOKEN_TRUE}, 3},
    {"var _123 = true_fake", {TOKEN_VAR, TOKEN_IDENTIFIER, TOKEN_EQUAL, TOKEN_IDENTIFIER}, 4},
    // false
    {"var _123 = false", {TOKEN_VAR, TOKEN_IDENTIFIER, TOKEN_EQUAL, TOKEN_FALSE}, 4},
    {"var _123 = true_fake", {TOKEN_VAR, TOKEN_IDENTIFIER, TOKEN_EQUAL, TOKEN_IDENTIFIER}, 4},
    // fn
    {"fn \nfunction() ", {TOKEN_FN, TOKEN_IDENTIFIER, TOKEN_LPAREN, TOKEN_RPAREN}, 4},
    // while
    {"while(true)", {TOKEN_WHILE, TOKEN_LPAREN, TOKEN_TRUE, TOKEN_RPAREN}, 4},
    // for 
    {"for(true)", {TOKEN_FOR, TOKEN_LPAREN, TOKEN_TRUE, TOKEN_RPAREN}, 4},
    // if else
    {"if(true)", {TOKEN_IF, TOKEN_LPAREN, TOKEN_TRUE, TOKEN_RPAREN}, 4},
    {"else return;", {TOKEN_ELSE, TOKEN_RETURN, TOKEN_SEMICOLON}, 3},
    // class
    {"class MyClass", {TOKEN_CLASS, TOKEN_IDENTIFIER}, 2},
    // this
    {"return this.true()", {TOKEN_RETURN, TOKEN_THIS, TOKEN_DOT, TOKEN_TRUE, TOKEN_LPAREN, TOKEN_RPAREN}, 6},
    // numbers
    {"return 23232.2323", {TOKEN_RETURN, TOKEN_DOUBLE}, 2},
    {"return 23", {TOKEN_RETURN, TOKEN_INTEGER}, 2},
    {"return -23", {TOKEN_RETURN, TOKEN_MINUS, TOKEN_INTEGER}, 3},
    {"2323 -23", {TOKEN_INTEGER, TOKEN_MINUS, TOKEN_INTEGER}, 3},
    {"2 .23", {TOKEN_INTEGER, TOKEN_DOT, TOKEN_INTEGER}, 3}, // not a valid number
    {"2.", {TOKEN_INTEGER, TOKEN_DOT}, 2},
    // {"2_23", {NUMBER}, 1}, // TODO add support of underscores for numbers ?


    // comments
    {"fn // comment return 2 + 2\n 222", {TOKEN_FN, TOKEN_SLASH_SLASH, TOKEN_INTEGER}, 3},
    {"fn // comment // inside another one return 2 + 2\n 222", {TOKEN_FN, TOKEN_SLASH_SLASH, TOKEN_INTEGER}, 3},
    {"fn // comment\n//\n//\n// //  2 + 2\n 222", {TOKEN_FN, TOKEN_SLASH_SLASH, TOKEN_SLASH_SLASH, TOKEN_SLASH_SLASH, TOKEN_SLASH_SLASH, TOKEN_INTEGER}, 6},
};

int amount = sizeof(test_cases) / sizeof(TestCase);

TestStatus lexer_test_case(char *code, TokenType expected[], int e_size);

TestResults run_lexer_test(int argc, const char *argv[]) {
    TestResults res = init_test_results(16);
    for (int i = 0; i < amount; i++) {
        TestCase c = test_cases[i];
        TestStatus s = lexer_test_case(c.input, c.expected, c.exp_size);
        add_result(&res, (TestResult){ .result = s });
    }
    return res;
}

TestStatus lexer_test_case(char *code, TokenType expected[], int e_size) {
    printf("Test scan string: '%s'\n", code);
    Tokens *t = scan(code, strlen(code));
    if (t->size != e_size + 1) {
        printf("\033[31mFAIL:\033[m Expected size does not equal scanned size. Expected: %d "
               "got: %d input string: \n'''\n%s\n'''\n",
               e_size, t->size, code);
        free_tokens(t);
        return FAIL;
    }
    for (int i = 0; i < t->size - 1; i++) {
        Token token = t->tokens[i];
        if (token.type != expected[i]) {
            printf("\033[31mFAIL:\033[m Expected token does not correspond to scanned. Expected '%s' got '%s'\n", token_name(expected[i]), token_name(token.type));
            free_tokens(t);
            return FAIL;
        }
    }
    Token last = t->tokens[t->size - 1];
    if(last.type != TOKEN_EOF) {
        printf("\033[31mFAIL:\033[m Expected token does not correspond to scanned. Expected '%s' got '%s'\n", token_name(TOKEN_EOF), token_name(last.type));
        free_tokens(t);
        return FAIL;
    }
    free_tokens(t);
    return PASS;
}
