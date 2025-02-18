#include "test_parser.h"
#include "uuc_assert.h"
#include "uuc_test.h"

void parser_test_case(TestResults *results, char *expecting, char *code);
void parser_test_case_for_comp_error(TestResults *results, char *code);

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
    parser_test_case(&res, "( = a ( + c d ) )", 
                           "a = c + d;");

    parser_test_case_for_comp_error(&res, "a + b = 23;");
    parser_test_case_for_comp_error(&res, "a +1= 23;");
    parser_test_case_for_comp_error(&res, "+a= 23;");
    return res;
}

void parser_test_case(TestResults *results, char *expecting, char *code) {
    char buf[300];
    printf("Test parse input expression: '%s'\n", code);
    Slice slice;
    UucResult pr = parse_code(&slice, code);
    if(pr != UUC_OK) {
        assert_fail("Unexpected parsing result\n");
        add_result(results, (TestResult){.result = FAIL});
        return;
    }
    slice_s_notation(&slice, buf, 300);
    printf("Result: %s\n", buf);
    int r = assert_str(expecting, buf);
    add_result(results, (TestResult){ .result = r ? FAIL : PASS });
}

void parser_test_case_for_comp_error(TestResults *results, char *code) {
    printf("Test compilation error for input: '%s'\n", code);
    Slice slice;
    UucResult pr = parse_code(&slice, code);
    if(pr != UUC_COMP_ERROR) {
        assert_fail("Expected compilation error.\n");
        add_result(results, (TestResult){.result = FAIL});
        return;
    }
    add_result(results, (TestResult){ .result = PASS });
}
