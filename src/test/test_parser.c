#include "test_parser.h"
#include "uuc_assert.h"
#include "uuc_test.h"

void parser_test_case(TestResults *results, char *expecting, char *code);

TestResults run_parser_test(int argc, const char *argv[]) {
    TestResults res = init_test_results(16);
    parser_test_case(&res, "( * 1 ( - 2 3 ) )", 
                           " 1 * (2 - 3);");
    parser_test_case(&res, "( - ( + 1 2 ) 3 )", 
                           "1  +2-3;");
    parser_test_case(&res, "( + ( * ( / 1 2 ) 3 ) 4 )", 
                           "1/2*3+4;");
    parser_test_case(&res, "( * ( * 1 ( + 2 3 ) ) 4 )", 
                           "1*(2+3)*4;");
    parser_test_case(&res, "( / 1 ( * 2 3 ) )", 
                           "1/(2*3);");
    parser_test_case(&res, "( / 1 ( * 2 3 ) )", 
                           "1/(2*3);");
    parser_test_case(&res, "( + ( - ( + 1 2 ) 3 ) 4 )", 
                           "1+2-3+4;");
    parser_test_case(&res, "( + 1 ( * 2 3 ) )", 
                           "1+2*3;");
    parser_test_case(&res, "( - 1 )", 
                           "-1;");
    parser_test_case(&res, "( - ( - 1 ) 2 )", 
                           "-1-2;");
    parser_test_case(&res, "( - ( - 1 ) ( ! 2 ) )", 
                           "-1- !2;");
    parser_test_case(&res, "( + ( - 1 ) ( - ( ! ( - 2 ) ) ) )", 
                           "-1+-!-2;");
    parser_test_case(&res, "( * 1 ( - 2 ) )", 
                           "(1*-2);");
    parser_test_case(&res, "( + ( ! 1 ) 2 )", 
                           "!1+2;");
    parser_test_case(&res, "( ! ( + 1 2 ) )", 
                           "!(1+2);");
    parser_test_case(&res, "( - ( ! ( - ( ! ( - 1 ) ) ) ) 2 )", 
                           "!-!-1-2;");
    parser_test_case(&res, "( - ( + 1 2 ) )", 
                           "-(1+2);");
    parser_test_case(&res, "( / ( / ( / 1 2 ) 3 ) 4 )", 
                           "1/2/3/4;");
    parser_test_case(&res, "( * ( / ( * ( / 1 2 ) 3 ) 4 ) 5 )", 
                           "1/2*3/4*5;");
    parser_test_case(&res, "( * ( / ( * ( / ( ! 1 ) ( ! 2 ) ) ( ! 3 ) ) ( - 4 ) ) ( - 5 ) )", 
                           "!1/!2*!3/-4*-5;");
    parser_test_case(&res, "( + 1 2.00 )", 
                           "1+2.0;");
    parser_test_case(&res, "( == ( ! true ) false )", 
                           "!true==false;");
    parser_test_case(&res, "( == ( + 2 2 ) 3 )", 
                           "2+2==3;");
    parser_test_case(&res, "( + \"string\" \"string\" )", 
                           "\"string\" + \"string\";");
    parser_test_case(&res, "( var a \"string\" )", 
                           "var a = \"string\";");
    parser_test_case(&res, "( var a ( + \"Hello\" \"World\" ) )", 
                           "var a = \"Hello\" + \"World\";");
    parser_test_case(&res, "( = a ( + \"Hello\" \"World\" ) )", 
                           "a = \"Hello\" + \"World\";");
    parser_test_case(&res, "( = ( + a b ) ( + c d ) )", 
                           "a + b = c + d;");
    return res;
}

void parser_test_case(TestResults *results, char *expecting, char *code) {
    char buf[300];
    printf("Test parse input expression: '%s'\n", code);
    Slice slice = parse_code(code);
    slice_s_notation(&slice, buf, 300);
    printf("Result: %s\n", buf);
    int r = assert_str(expecting, buf);
    add_result(results, (TestResult){ .result = r ? FAIL : PASS });
}
