#ifndef lexer_test_h 
#define lexer_test_h

#include "../lexer.h"

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

int run_lexer_test(int argc, const char *argv[]);
void test(char *code, int size, TokenType expected[], int e_size);

#endif
