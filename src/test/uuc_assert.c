#include <math.h>
#include <stdio.h>
#include "uuc_assert.h"
#include "../include/uuc_type.h"

const char *bool_str(int bool) {
    return bool ? "true" : "false";
}

int assert_str(char *expected, char *test) {
    int i = 0;
    char e = expected[i];
    char t = test[i];
    while(1) {
        if(e != t) {
            printf("\033[31mAssertion error: string are different at %d:\033[m \n", i);
            // TODO add coloring (?) & handle new lines or this can become an unreadable mess
            printf("  expected: %s\n", expected);
            printf("       got: %s\n", test);
            return 1;
        }
        if(e == '\0' || t == '\0') return 0;
        i++;
        e = expected[i];
        t = test[i];
    }
    return 0;
}

int assert_null(Value val) {
    return val.type == TYPE_NULL;
}

int assert_value(Value right, Value left) {
    if(assert_type(right.type, left)) return 1;
    switch(right.type) {
        case TYPE_INT: return assert_integer(right.as.uuc_int, left.as.uuc_int);
        case TYPE_DOUBLE: return assert_double(right.as.uuc_double, left.as.uuc_double);
        case TYPE_BOOL: return assert_bool(right.as.uuc_bool, left.as.uuc_bool);
        default: return 1;
    }
    return 0;
}

int assert_type(UucType expected, Value val) {
    if(val.type != expected) {
        printf("\033[31mAssertion error: type mistmatch -- (right) %s is not (left) %s\033[m\n", 
               uuc_type_str(expected), uuc_type_str(val.type));
        return 1;
    }
    return 0;
}

int assert_is_integer(Value val) {
    return assert_type(TYPE_INT, val);
}

int assert_integer(int expected, int test) {
    if(expected != test) {
        printf("\033[31mAssertion error: numbers are different -- (right) %d != (left) %d\033[m\n", expected, test);
        return 1;
    }
    return 0;
}

int assert_bool(int right, int left) {
    if(right != left) {
        printf("\033[31mAssertion error: booleans are different -- (right) %s != (left) %s\033[m\n", bool_str(right), bool_str(left));
        return 1;
    }
    return 0;
}

int assert_double_d(double expected, double test, double delta) {
    if(fabs(expected - test) > delta) {
        printf("\033[31mAssertion error: numbers are different -- (right) %f != (left) %f\033[m\n", expected, test);
        return 1;
    }
    return 0;
}
