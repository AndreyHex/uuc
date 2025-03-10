#include <math.h>
#include <stdio.h>
#include "uuc_assert.h"
#include "../include/uuc_type.h"

const char *bool_str(int bool) {
    return bool ? "true" : "false";
}

int assert_fail(char *msg) {
    printf("\033[31mAssertion error: '%s'\033[m \n", msg);
    return 1;
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
        if(e == '\0' || t == '\0') return t != e;
        i++;
        e = expected[i];
        t = test[i];
    }
    return 0;
}

int assert_null(Value val) {
    return val.type == TYPE_NULL;
}

int assert_value(Value left, Value right) {
    if(assert_type(left.type, right)) return 1;
    switch(left.type) {
        case TYPE_INT: return assert_integer(left.as.uuc_int, right.as.uuc_int);
        case TYPE_DOUBLE: return assert_double(left.as.uuc_double, right.as.uuc_double);
        case TYPE_BOOL: return assert_bool(left.as.uuc_bool, right.as.uuc_bool);
        case TYPE_NULL: return assert_type(TYPE_NULL, right);
        case TYPE_OBJ: return assert_obj(left.as.uuc_obj, right.as.uuc_obj);
        default: {
            printf("Unknown type for assertion\n");
            return 1;
        }
    }
    return 0;
}

int assert_obj(UucObj *left, UucObj *right) {
    if(right->type != left->type) {
        printf("\033[31mAssertion error: obj type mistmatch -- (left) %d is not (right) %d\033[m\n", left->type, right->type); // only strings now
        return 1;
    }
    switch(left->type) {
        case OBJ_STRING: return assert_str(((UucString*)left)->content, ((UucString*)right)->content);
        default: return 0;
    }
}

int assert_type(UucType expected, Value val) {
    if(val.type != expected) {
        printf("\033[31mAssertion error: type mistmatch -- (left) %s is not (right) %s\033[m\n", 
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
        printf("\033[31mAssertion error: numbers are different -- (left) %d != (right) %d\033[m\n", expected, test);
        return 1;
    }
    return 0;
}

int assert_bool(int left, int right) {
    if(left != right) {
        printf("\033[31mAssertion error: booleans are different -- (left) %s != (right) %s\033[m\n", bool_str(left), bool_str(right));
        return 1;
    }
    return 0;
}

int assert_double_d(double expected, double test, double delta) {
    if(fabs(expected - test) > delta) {
        printf("\033[31mAssertion error: numbers are different -- (left) %f != (right) %f\033[m\n", expected, test);
        return 1;
    }
    return 0;
}
