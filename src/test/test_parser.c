#include "test_parser.h"
#include "uuc_assert.h"
#include "uuc_test.h"

void parser_test_case(TestResults *results, char *expecting, char *code);

TestResults run_parser_test(int argc, const char *argv[]) {
    TestResults res = init_test_results(16);
    parser_test_case(&res, "( OP_MULTIPLY 1.00 ( OP_SUBSTRACT 2.00 3.00 ) )", 
                           " 1 * (2 - 3);");
    parser_test_case(&res, "( OP_ADD 1.00 ( OP_SUBSTRACT 2.00 3.00 ) )", 
                           "1  +2-3;");
    return res;
}

void parser_test_case(TestResults *results, char *expecting, char *code) {
    char buf[300];
    Slice slice = parse_code(code);
    slice_s_notation(&slice, buf, 300);
    int r = assert_str(expecting, buf);
    add_result(results, (TestResult){ .result = r ? FAIL : PASS });
}
