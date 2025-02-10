#include <math.h>
#include <stdio.h>
#include "../include/uuc_type.h"

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

int assert_type(UucType expected, Value val) {
    if(val.type != expected) {
        printf("\033[31mAssertion error: type mistmatch -- (right) %d is not (left) %d\033[m\n", expected, val.type);
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

int assert_double_d(double expected, double test, double delta) {
    if(fabs(expected - test) > delta) {
        printf("\033[31mAssertion error: numbers are different -- (right) %f != (left) %f\033[m\n", expected, test);
        return 1;
    }
    return 0;
}
