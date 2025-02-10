#include "test_parser.h"
#include "uuc_assert.h"
#include "uuc_test.h"

void parser_test_case(TestResults *results, char *expecting, char *code);

TestResults run_parser_test(int argc, const char *argv[]) {
    TestResults res = init_test_results(16);
    parser_test_case(&res, "( * 1.00 ( - 2.00 3.00 ) )", 
                           " 1 * (2 - 3);");
    parser_test_case(&res, "( - ( + 1.00 2.00 ) 3.00 )", 
                           "1  +2-3;");
    parser_test_case(&res, "( + ( * ( / 1.00 2.00 ) 3.00 ) 4.00 )", 
                           "1/2*3+4;");
    parser_test_case(&res, "( * ( * 1.00 ( + 2.00 3.00 ) ) 4.00 )", 
                           "1*(2+3)*4;");
    parser_test_case(&res, "( / 1.00 ( * 2.00 3.00 ) )", 
                           "1/(2*3);");
    parser_test_case(&res, "( / 1.00 ( * 2.00 3.00 ) )", 
                           "1/(2*3);");
    parser_test_case(&res, "( + ( - ( + 1.00 2.00 ) 3.00 ) 4.00 )", 
                           "1+2-3+4;");
    parser_test_case(&res, "( + 1.00 ( * 2.00 3.00 ) )", 
                           "1+2*3;");
    parser_test_case(&res, "( - 1.00 )", 
                           "-1;");
    parser_test_case(&res, "( - ( - 1.00 ) 2.00 )", 
                           "-1-2;");
    parser_test_case(&res, "( - ( - 1.00 ) ( ! 2.00 ) )", 
                           "-1- !2;");
    parser_test_case(&res, "( + ( - 1.00 ) ( - ( ! ( - 2.00 ) ) ) )", 
                           "-1+-!-2;");
    parser_test_case(&res, "( * 1.00 ( - 2.00 ) )", 
                           "(1*-2);");
    parser_test_case(&res, "( + ( ! 1.00 ) 2.00 )", 
                           "!1+2;");
    parser_test_case(&res, "( ! ( + 1.00 2.00 ) )", 
                           "!(1+2);");
    parser_test_case(&res, "( - ( ! ( - ( ! ( - 1.00 ) ) ) ) 2.00 )", 
                           "!-!-1-2;");
    parser_test_case(&res, "( - ( + 1.00 2.00 ) )", 
                           "-(1+2);");
    parser_test_case(&res, "( / ( / ( / 1.00 2.00 ) 3.00 ) 4.00 )", 
                           "1/2/3/4;");
    parser_test_case(&res, "( * ( / ( * ( / 1.00 2.00 ) 3.00 ) 4.00 ) 5.00 )", 
                           "1/2*3/4*5;");
    parser_test_case(&res, "( * ( / ( * ( / ( ! 1.00 ) ( ! 2.00 ) ) ( ! 3.00 ) ) ( - 4.00 ) ) ( - 5.00 ) )", 
                           "!1/!2*!3/-4*-5;");
    return res;
}

void parser_test_case(TestResults *results, char *expecting, char *code) {
    char buf[300];
    printf("Input expression: '%s'\n", code);
    Slice slice = parse_code(code);
    slice_s_notation(&slice, buf, 300);
    int r = assert_str(expecting, buf);
    add_result(results, (TestResult){ .result = r ? FAIL : PASS });
}
