#include <math.h>
#include <stdio.h>

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

int assert_double_d(double expected, double test, double delta) {
    if(fabs(expected - test) > delta) {
        printf("\033[31mAssertion error: numbers are different -- (right) %f != (left) %f\033[m\n", expected, test);
        return 1;
    }
    return 0;
}
