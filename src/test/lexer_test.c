#include "uuc_test.h"
#include "../lexer.h"
#include <string.h>
#include <stdio.h>

typedef struct {
    char *input;
    TokenType expected[100];
    int exp_size;
} TestCase;

TestCase test_cases[] = {
    {"++--", {PLUS_PLUS, MINUS_MINUS}, 2},
    {"+-", {PLUS, MINUS}, 2},
    {"(())", {LEFT_PAREN, LEFT_PAREN, RIGHT_PAREN, RIGHT_PAREN}, 4},
    {"({})", {LEFT_PAREN, LEFT_BRACE, RIGHT_BRACE, RIGHT_PAREN}, 4},
    {"&&&", {AND_AND, AND}, 2},
    {"&&=&", {AND_AND, EQUAL, AND}, 3},
    {"&&=&=", {AND_AND, EQUAL, AND_EQUAL}, 3},
    {"..+-/=", {DOT, DOT, PLUS, MINUS, SLASH_EQUAL}, 5},
    {"..//+-/=\n+", {DOT, DOT, SLASH_SLASH, PLUS}, 4},
    {"\"string\"", {STRING}, 1},
    {"\"st\\\"ring\"", {STRING}, 1},
    {"(", {LEFT_PAREN}, 1},

    // var and identifiers
    {"var identifier =", {VAR, IDENTIFIER, EQUAL}, 3},
    {"var ident111ifier", {VAR, IDENTIFIER}, 2},
    {"var _", {VAR, IDENTIFIER}, 2},
    {"var _123", {VAR, IDENTIFIER}, 2},
    {"var _1dSASSds_ds23", {VAR, IDENTIFIER}, 2},
    {"var __1_23", {VAR, IDENTIFIER}, 2},
    {"var_123 _", {IDENTIFIER, IDENTIFIER}, 2},
    // keyword is cut
    {"var tru", {VAR, IDENTIFIER}, 2},
    {"var tru\ne", {VAR, IDENTIFIER, IDENTIFIER}, 3},
    // true
    {"var _123 = true", {VAR, IDENTIFIER, EQUAL, TRUE}, 4},
    {"var _123 = true_fake", {VAR, IDENTIFIER, EQUAL, IDENTIFIER}, 4},
    // false
    {"var _123 = false", {VAR, IDENTIFIER, EQUAL, FALSE}, 4},
    {"var _123 = true_fake", {VAR, IDENTIFIER, EQUAL, IDENTIFIER}, 4},
    // fn
    {"fn \nfunction() ", {FN, IDENTIFIER, LEFT_PAREN, RIGHT_PAREN}, 4},
    // while
    {"while(true)", {WHILE, LEFT_PAREN, TRUE, RIGHT_PAREN}, 4},
    // for 
    {"for(true)", {FOR, LEFT_PAREN, TRUE, RIGHT_PAREN}, 4},
    // if else
    {"if(true)", {IF, LEFT_PAREN, TRUE, RIGHT_PAREN}, 4},
    {"else return;", {ELSE, RETURN, SEMICOLON}, 3},
    // class
    {"class MyClass", {CLASS, IDENTIFIER}, 2},
    // this
    {"return this.true()", {RETURN, THIS, DOT, TRUE, LEFT_PAREN, RIGHT_PAREN}, 6},
    // numbers
    {"return 23232.2323", {RETURN, NUMBER}, 2},
    {"return 23", {RETURN, NUMBER}, 2},
    {"return -23", {RETURN, MINUS, NUMBER}, 3},
    {"2323 -23", {NUMBER, MINUS, NUMBER}, 3},
    {"2 .23", {NUMBER, DOT, NUMBER}, 3}, // not a valid number
    // {"2_23", {NUMBER}, 1}, // TODO add support of underscores for numbers ?
};

int amount = sizeof(test_cases) / sizeof(TestCase);
int passed = 0;

void test(char *code, TokenType expected[], int e_size);

int run_lexer_test(int argc, const char *argv[]) {
    for (int i = 0; i < amount; i++) {
        TestCase c = test_cases[i];
        test(c.input, c.expected, c.exp_size);
    }
    if(passed == amount) {
        printf("Tests results:\n  Test cases: %d\n  \033[32mPassed: %d\033[m\n  Failed: %d\n", amount, passed, amount - passed);
    } else {
        printf("Tests results:\n  Test cases: %d\n  \033[32mPassed: %d\033[m\n  \033[31mFailed: %d\033[m\n", amount, passed, amount - passed);
    }
    return 0;
}

void test(char *code, TokenType expected[], int e_size) {
    Tokens *t = scan(code, strlen(code));
    if (t->count != e_size) {
        printf("\033[31mFAIL:\033[m Expected size does not equal scanned size. Expected: %d "
               "got: %d input string: \n'''\n%s\n'''\n",
               e_size, t->count, code);
        free_tokens(t);
        return;
    }
    for (int i = 0; i < t->count; i++) {
        Token token = t->tokens[i];
        if (token.type != expected[i]) {
            printf("\033[31mFAIL:\033[m Expected token does not correspond to scanned. Expected '%s' got '%s'\n", token_name(expected[i]), token_name(token.type));
            free_tokens(t);
            return;
        }
    }
    // printf("Passed for: '%s'\n", code);
    passed++;
}
