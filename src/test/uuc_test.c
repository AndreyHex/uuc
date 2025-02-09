#include "uuc_test.h"
#include <stdlib.h>
#include "../include/memory.h"
#include "../include/log.h"

TestResults init_test_results(uint64_t initial_capacity) {
    void *ptr = malloc(initial_capacity * sizeof(TestResult));
    return (TestResults){
        .results = ptr,
        .capacity = initial_capacity,
        .size = 0,
    };
}

int add_result(TestResults *results, TestResult result) {
    if(results->size == results->capacity) {
        int new_cap = INCREASE_CAPACITY(results->capacity);
        LOG_TRACE("Increasing TestResults size from %d to %d\n", results->capacity, new_cap);
        results->results = INCREASE_ARRAY(TestResult, results->results, results->capacity, new_cap);
        results->capacity = new_cap;
    }
    results->results[results->size] = result;
    results->size++;
    return results->size - 1;
}
