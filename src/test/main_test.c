#include "test_lexer.h"
#include "test_compiler.h"
#include "test_vm.h"
#include "test_val_table.h"
#include "uuc_test.h"

void summarize_tests(TestResults results, int *passed, int *all) {
    int p = 0;
    int a = 0;
    for(int i = 0; i < results.size; i++) {
        TestResult r = results.results[i];
        a++;
        if(r.result == PASS) p++;
    }
    *all = *all + a;
    *passed = *passed + p;

}

int main(int argc, const char *argv[]) {
    TestResults lexer_res = run_lexer_test(argc, argv);
    TestResults compiler_res = run_compiler_test(argc, argv);
    TestResults vm_res = run_vm_test(argc, argv);
    TestResults table_res = run_val_table_tests(argc, argv);
    
    int passed = 0;
    int all = 0;
    summarize_tests(compiler_res, &passed, &all);
    summarize_tests(lexer_res, &passed, &all);
    summarize_tests(vm_res, &passed, &all);
    summarize_tests(table_res, &passed, &all);

    if(passed == all) {
        printf("Tests results:\n  Test cases: %d\n  \033[32mPassed: %d\033[m\n  Failed: %d\n", all, passed, all - passed);
    } else {
        printf("Tests results:\n  Test cases: %d\n  \033[32mPassed: %d\033[m\n  \033[31mFailed: %d\033[m\n", all, passed, all - passed);
    }
}
