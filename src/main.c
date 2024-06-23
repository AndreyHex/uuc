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

void test_case(char *code, int size, TokenType expected[], int e_size);
TestResult test(char *code, int size, TokenType expected[], int e_size);

int main(int argc, const char *argv[]) {

    char *code = "++==\0";
    TokenType expected[] = {PLUS_PLUS, EQUAL_EQUAL};
    test_case(code, 4, expected, 2);

    code = "+++\0";
    TokenType expected1[] = {PLUS_PLUS, PLUS};
    test_case(code, 3, expected1, 2);

    code = "+++=!!=&&=\0";
    TokenType expected2[] = {PLUS_PLUS, PLUS_EQUAL, BANG, BANG_EQUAL, AND_AND, EQUAL};
    test_case(code, 10, expected2, 6);

    return 0;
}

void test_case(char *code, int size, TokenType expected[], int e_size) {
    printf("%s\n", "========TEST CASE========");
    printf("input: '%s'\n", code);
    TestResult t = test(code, size, expected, e_size);
    if (t.result == FAIL) {
        printf("FAIL: '%s'\n", t.message);
    } else {
        printf("PASS\n");
    }
}

TestResult test(char *code, int size, TokenType expected[], int e_size) {
    TestResult r;
    Tokens *t = scan(code, size);

    /*
    for(int i = 0; i< t->count; i++ ) printf("_%u", t->tokens[i].type);
    printf("\n");
    */

    if (t->count != e_size) {
        r.result = FAIL;
        r.message = "Expected size does not equal scanned size";
        free_tokens(t);
        return r;
    }

    for (int i = 0; i < t->count; i++) {
        Token token = t->tokens[i];
        if (token.type != expected[i]) {
            r.result = FAIL;
            r.message = "Expected token does not correspond to scanned";
            free_tokens(t);
            return r;
        }
    }
    r.result = PASS;
    return r;
}
