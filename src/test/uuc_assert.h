#ifndef uuc_assert_h
#define uuc_assert_h

#include "../include/uuc_type.h"

// returns 0 if strings are the same
int assert_str(char *expected, char *test);

// returs 0 if numbers are the same
int assert_integer(int expected, int test);

int assert_is_integer(Value val);

// returs 0 if numbers are the same
int assert_double_d(double expected, double test, double delta);

static int assert_double(double expected, double test) {
    return assert_double_d(expected, test, 0);
}

#endif
