#ifndef uuc_text_h 
#define uuc_text_h

#include <stdint.h>

typedef enum {
    PASS,
    FAIL,
} TestStatus;

typedef struct {
    char *message;
    TestStatus result;
} TestResult;

typedef struct {
    TestResult *results;
    uint64_t size;
    uint64_t capacity;
} TestResults;

TestResults init_test_results(uint64_t initial_capacity);
int add_result(TestResults *results, TestResult result);

#endif
