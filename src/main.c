#include "lexer.h"
#include <stdio.h>

typedef enum {
    PASS,
    FAIL,
} TestStatus;

typedef struct {
    char *message;
    TestStatus result;
} TestResult;

typedef struct {
    char *input;
    int input_size;
    TokenType expected[100];
    int exp_size;
} TestCase;

TestCase test_cases[] = {
    {"++--\0", 5, {PLUS_PLUS, MINUS_MINUS}, 2},
    {"+-\0", 3, {PLUS, MINUS}, 2},
    {"(())\0", 5, {LEFT_PAREN, LEFT_PAREN, RIGHT_PAREN, RIGHT_PAREN}, 4},
    {"({})\0", 5, {LEFT_PAREN, LEFT_BRACE, RIGHT_BRACE, RIGHT_PAREN}, 4},
    {"&&&\0", 4, {AND_AND, AND}, 2},
    {"&&=&\0", 5, {AND_AND, EQUAL, AND}, 3},
    {"&&=&=\0", 5, {AND_AND, EQUAL, AND_EQUAL}, 3},
    {"..+-/=\0", 7, {DOT, DOT, PLUS, MINUS, SLASH_EQUAL}, 5},
    {"..//+-/=\n+\0", 10, {DOT, DOT, SLASH_SLASH, PLUS}, 4},
    {"\"string\"\0", 9, {STRING}, 1},
    {"\"st\\\"ring\"\0", 11, {STRING}, 1},
};

int amount = sizeof(test_cases) / sizeof(TestCase);
int passed = 0;

void test(char *code, int size, TokenType expected[], int e_size);

int main(int argc, const char *argv[]) {
    for (int i = 0; i < amount; i++) {
        TestCase c = test_cases[i];
        test(c.input, c.input_size, c.expected, c.exp_size);
    }
    printf("Tests result: %d/%d\n", passed, amount);
    return 0;
}

void test(char *code, int size, TokenType expected[], int e_size) {
    printf("%s\n", "========TEST CASE========");
    printf("input: '%s'\n", code);
    Tokens *t = scan(code, size);
    if (t->count != e_size) {
        printf("FAIL: Expected size does not equal scanned size. Expected: %d got: %d\n", e_size, t->count);
        free_tokens(t);
        return;
    }

    for (int i = 0; i < t->count; i++) {
        Token token = t->tokens[i];
        if (token.type != expected[i]) {
            printf("FAIL: Expected token does not correspond to scanned.\n");
            free_tokens(t);
            return;
        }
    }
    printf("PASS\n");
    passed++;
}
