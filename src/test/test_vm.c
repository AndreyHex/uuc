#include "test_vm.h"
#include "uuc_assert.h"
#include "uuc_test.h"

#define INT_VAL(v) (Value){ .type = TYPE_INT, .as = { .uuc_int = v } }
#define DOUBLE_VAL(v) (Value){ .type = TYPE_DOUBLE, .as = { .uuc_double = v } }
#define BOOL_TRUE (Value){ .type = TYPE_BOOL, .as = { .uuc_bool = 1 } }
#define BOOL_FALSE (Value){ .type = TYPE_BOOL, .as = { .uuc_bool = 0 } }

TestResult run_vm_test_case(Value expecting, char *code);
TestResult run_vm_error_test_case(ExeResult expected_res, char *code);

TestResults run_vm_test(int argc, const char *argv[]) {
    TestResults r = init_test_results(16);

    add_result(&r, run_vm_test_case(INT_VAL(26), "1 + (2 + 3) * 4 + 5;"));
    add_result(&r, run_vm_test_case(INT_VAL(18), "3*(3+3);"));
    add_result(&r, run_vm_test_case(INT_VAL(11), "3*3+2;"));
    add_result(&r, run_vm_test_case(INT_VAL(0), "-(-(2*1+-2));"));
    add_result(&r, run_vm_test_case(INT_VAL(2), "4/2;"));
    add_result(&r, run_vm_test_case(INT_VAL(1), "4/2/2;"));
    add_result(&r, run_vm_test_case(INT_VAL(2), "4/2/2*2;"));
    add_result(&r, run_vm_test_case(INT_VAL(-2), "2-4;"));
    add_result(&r, run_vm_test_case(INT_VAL(-8), "2-4-6;"));
    add_result(&r, run_vm_test_case(INT_VAL(4), "2*2*2/2;"));

    add_result(&r, run_vm_test_case(DOUBLE_VAL(0.25), "1/4.0;"));
    add_result(&r, run_vm_test_case(DOUBLE_VAL(4.0), "1*4.0;"));
    add_result(&r, run_vm_test_case(DOUBLE_VAL(3.0), "1.0+2;"));
    add_result(&r, run_vm_test_case(DOUBLE_VAL(-0.5), "1-1.5;"));

    add_result(&r, run_vm_test_case(BOOL_TRUE, "true;"));
    add_result(&r, run_vm_test_case(BOOL_TRUE, "!false;"));
    add_result(&r, run_vm_test_case(BOOL_FALSE, "false;"));
    add_result(&r, run_vm_test_case(BOOL_FALSE, "!true;"));

    add_result(&r, run_vm_error_test_case(UUC_RUNTIME_ERROR, "2*true*2/2;"));
    add_result(&r, run_vm_error_test_case(UUC_RUNTIME_ERROR, "2/0;"));
    add_result(&r, run_vm_error_test_case(UUC_RUNTIME_ERROR, "2/true;"));
    add_result(&r, run_vm_error_test_case(UUC_RUNTIME_ERROR, "false-true;"));
    add_result(&r, run_vm_error_test_case(UUC_RUNTIME_ERROR, "false/false;"));
    add_result(&r, run_vm_error_test_case(UUC_RUNTIME_ERROR, "false*true;"));
    add_result(&r, run_vm_error_test_case(UUC_RUNTIME_ERROR, "-false;"));
    add_result(&r, run_vm_error_test_case(UUC_RUNTIME_ERROR, "!10;"));
    add_result(&r, run_vm_error_test_case(UUC_RUNTIME_ERROR, "10+null;"));
    add_result(&r, run_vm_error_test_case(UUC_RUNTIME_ERROR, "10-13-null;"));
    add_result(&r, run_vm_error_test_case(UUC_RUNTIME_ERROR, "10*13/null;"));
    add_result(&r, run_vm_error_test_case(UUC_RUNTIME_ERROR, "10+2*null;"));
    add_result(&r, run_vm_error_test_case(UUC_RUNTIME_ERROR, "!(null);"));
    add_result(&r, run_vm_error_test_case(UUC_RUNTIME_ERROR, "-null;"));
    add_result(&r, run_vm_error_test_case(UUC_RUNTIME_ERROR, "2+-null;"));

    return r;
}

TestResult run_vm_test_case(Value expecting, char *code) {
    Slice slice = parse_code(code);
    VM vm = vm_init(slice);
    vm_run(&vm);
#if defined(UUC_LOG_TRACE)
    slice_print(&slice);
#endif
    Value r = stack_peek(&vm.value_stack);
    printf("Test execute expression: '%s' => ", code);
    type_print(r);
    printf("\n");
    if(assert_value(expecting, r)) {
        return (TestResult){ .result = FAIL };
    }
    return (TestResult){ .result = PASS };
}

TestResult run_vm_error_test_case(ExeResult expected_res, char *code) {
    printf("Test execute expression for error: '%s'\n", code);
    Slice slice = parse_code(code);
    VM vm = vm_init(slice);
    ExeResult r = vm_run(&vm);
#if defined(UUC_LOG_TRACE)
    slice_print(&slice);
#endif
    if(r == 0) {
        printf("\033[31mAssertion error: expected runtime error for input '%s'\033[m\n", code);
        return (TestResult){ .result = FAIL };
    }
    return (TestResult){ .result = PASS };
}

#undef INT_VAL
#undef DOUBLE_VAL
#undef BOOL_TRUE 
#undef BOOL_FALSE 
