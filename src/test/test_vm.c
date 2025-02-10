#include "test_vm.h"
#include "uuc_assert.h"
#include "uuc_test.h"

TestResult run_vm_test_case(double expecting, char *code);

TestResults run_vm_test(int argc, const char *argv[]) {
    TestResults r = init_test_results(16);

    add_result(&r, run_vm_test_case(26, "1 + (2 + 3) * 4 + 5;"));
    add_result(&r, run_vm_test_case(18, "3*(3+3);"));
    add_result(&r, run_vm_test_case(11, "3*3+2;"));
    add_result(&r, run_vm_test_case(0, "-(-(2*1+-2));"));
    add_result(&r, run_vm_test_case(2, "4/2;"));
    add_result(&r, run_vm_test_case(1, "4/2/2;"));
    add_result(&r, run_vm_test_case(2, "4/2/2*2;"));
    add_result(&r, run_vm_test_case(-2, "2-4;"));
    add_result(&r, run_vm_test_case(-8, "2-4-6;"));
    add_result(&r, run_vm_test_case(4, "2*2*2/2;"));

    return r;
}

TestResult run_vm_test_case(double expecting, char *code) {
    Slice slice = parse_code(code);
    VM vm = vm_init(slice);
    vm_run(&vm);
#if defined(UUC_LEVEL_TRACE)
    stack_print(&vm.value_stack);
#endif
    double r = *vm.value_stack.tail;
    printf("'%s' => %f\n", code, r);
    if(assert_double(expecting, r)) {
        return (TestResult){ .result = FAIL };
    }
    return (TestResult){ .result = PASS };
}
