#include "test_vm.h"
#include "uuc_assert.h"
#include "uuc_test.h"
#include "../include/uuc_string.h"

#define INT_VAL(v) (Value){ .type = TYPE_INT, .as = { .uuc_int = v } }
#define NULL_VAL (Value){ .type = TYPE_NULL, .as = {0} }
#define DOUBLE_VAL(v) (Value){ .type = TYPE_DOUBLE, .as = { .uuc_double = v } }
#define BOOL_TRUE (Value){ .type = TYPE_BOOL, .as = { .uuc_bool = 1 } }
#define BOOL_FALSE (Value){ .type = TYPE_BOOL, .as = { .uuc_bool = 0 } }
#define STRING_OBJ(str) (Value){ .type = TYPE_OBJ, .as = { .uuc_obj = (UucObj*)uuc_create_string(str) } }

TestResult run_vm_test_case(Value expecting, char *code);
TestResult run_vm_error_test_case(UucResult expected_res, char *code);

TestResults run_vm_test(int argc, const char *argv[]) {
    TestResults r = init_test_results(16);

    add_result(&r, run_vm_test_case(INT_VAL(26), "var a = 1 + (2 + 3) * 4 + 5;"));
    add_result(&r, run_vm_test_case(INT_VAL(18), "var a = 3*(3+3);"));
    add_result(&r, run_vm_test_case(INT_VAL(11), "var a = 3*3+2;"));
    add_result(&r, run_vm_test_case(INT_VAL(0), "var a = -(-(2*1+-2));"));
    add_result(&r, run_vm_test_case(INT_VAL(2), "var a = 4/2;"));
    add_result(&r, run_vm_test_case(INT_VAL(1), "var a = 4/2/2;"));
    add_result(&r, run_vm_test_case(INT_VAL(2), "var a = 4/2/2*2;"));
    add_result(&r, run_vm_test_case(INT_VAL(-2), "var a = 2-4;"));
    add_result(&r, run_vm_test_case(INT_VAL(-8), "var a = 2-4-6;"));
    add_result(&r, run_vm_test_case(INT_VAL(4), "var a = 2*2*2/2;"));
    add_result(&r, run_vm_test_case(INT_VAL(-8862), "var a = 2*2*2/2-3-4-5-3+4+2+2+5+5*5/6*9*8*8/9-9+6+5*5-8*6-5-4-4/4*5-4-4*4*4-4-5-5*7*4*4*4*4-5-5*4-4-5-4-5;"));
    add_result(&r, run_vm_test_case(INT_VAL(4), "var a = 2; a = a+2;"));

    add_result(&r, run_vm_test_case(BOOL_TRUE, "var a = 2==2;"));
    add_result(&r, run_vm_test_case(BOOL_FALSE, "var a = 2==2.1;"));
    add_result(&r, run_vm_test_case(BOOL_FALSE, "var a = 2==3;"));
    add_result(&r, run_vm_test_case(BOOL_TRUE, "var a = 2!=3;"));
    add_result(&r, run_vm_test_case(BOOL_FALSE, "var a = 2!=2;"));
    add_result(&r, run_vm_test_case(BOOL_TRUE, "var a = true!=false;"));
    add_result(&r, run_vm_test_case(BOOL_FALSE, "var a = !true!=false;"));

    add_result(&r, run_vm_test_case(BOOL_FALSE, "var a = true>true;"));
    add_result(&r, run_vm_test_case(BOOL_FALSE, "var a = false>false;"));
    add_result(&r, run_vm_test_case(BOOL_TRUE, "var a = true>false;"));
    add_result(&r, run_vm_test_case(BOOL_TRUE, "var a = true>=false;"));
    add_result(&r, run_vm_test_case(BOOL_FALSE, "var a = true<false;"));
    add_result(&r, run_vm_test_case(BOOL_FALSE, "var a = true<=false;"));

    add_result(&r, run_vm_test_case(BOOL_TRUE, "var a = 2<=2.4;"));
    add_result(&r, run_vm_test_case(BOOL_TRUE, "var a = 2<=2;"));
    add_result(&r, run_vm_test_case(BOOL_FALSE, "var a = 3>=4;"));
    add_result(&r, run_vm_test_case(BOOL_FALSE, "var a = 3>4;"));

    add_result(&r, run_vm_test_case(DOUBLE_VAL(0.25), "var a = 1/4.0;"));
    add_result(&r, run_vm_test_case(DOUBLE_VAL(4.0), "var a = 1*4.0;"));
    add_result(&r, run_vm_test_case(DOUBLE_VAL(3.0), "var a = 1.0+2;"));
    add_result(&r, run_vm_test_case(DOUBLE_VAL(-0.5), "var a = 1-1.5;"));

    add_result(&r, run_vm_test_case(BOOL_TRUE, "var a = true;"));
    add_result(&r, run_vm_test_case(BOOL_TRUE, "var a = !false;"));
    add_result(&r, run_vm_test_case(BOOL_FALSE, "var a = false;"));
    add_result(&r, run_vm_test_case(BOOL_FALSE, "var a = !true;"));

    add_result(&r, run_vm_test_case(STRING_OBJ("test"), "var a = \"test\";"));
    add_result(&r, run_vm_test_case(STRING_OBJ("tt"), "var a = \"t\"+\"t\";"));
    add_result(&r, run_vm_test_case(STRING_OBJ("HelloWorld!!!"), "var a = \"Hello\"+\"World\"+\"!!!\";"));
    add_result(&r, run_vm_test_case(STRING_OBJ("aa"), "var a = \"a\"; a = a + a;"));

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
    add_result(&r, run_vm_error_test_case(UUC_RUNTIME_ERROR, "2==null;"));
    add_result(&r, run_vm_error_test_case(UUC_RUNTIME_ERROR, "2==true;"));
    add_result(&r, run_vm_error_test_case(UUC_RUNTIME_ERROR, "null==true;"));
    add_result(&r, run_vm_error_test_case(UUC_RUNTIME_ERROR, "true>2;"));
    add_result(&r, run_vm_error_test_case(UUC_RUNTIME_ERROR, "true<=2;"));
    add_result(&r, run_vm_error_test_case(UUC_RUNTIME_ERROR, "null<=2;"));
    add_result(&r, run_vm_error_test_case(UUC_RUNTIME_ERROR, "null<=(2*2+2);"));
    add_result(&r, run_vm_error_test_case(UUC_RUNTIME_ERROR, "\"str\">\"stt\";"));
    add_result(&r, run_vm_error_test_case(UUC_RUNTIME_ERROR, "\"str\">23;"));

    add_result(&r, run_vm_error_test_case(UUC_COMP_ERROR, "a + b = 23;"));

    // if
    add_result(&r, run_vm_test_case(INT_VAL(4), "var a = 1; if(true) { a = 4; }"));
    add_result(&r, run_vm_test_case(INT_VAL(1), "var a = 1; if(false) { a = 4; }"));
    add_result(&r, run_vm_test_case(INT_VAL(4), "var a = 1; if(2 > 1) { a = 4; }"));
    add_result(&r, run_vm_test_case(INT_VAL(1), "var a = 1; if(2 == 1) { a = 4; }"));
    add_result(&r, run_vm_test_case(INT_VAL(4), "var a = 1; if(true) a = 4;"));
    add_result(&r, run_vm_test_case(INT_VAL(1), "var a = 1; if(0) a = 4;"));
    add_result(&r, run_vm_test_case(INT_VAL(1), "var a = 1; if(2 + 3 > 4*7) a = 4;"));
    add_result(&r, run_vm_test_case(INT_VAL(4), "var a = 1; if(a) a = 4;"));
    add_result(&r, run_vm_test_case(BOOL_FALSE, "var a = false; if(a) a = 4;"));
    // if -> runtime error
    add_result(&r, run_vm_error_test_case(UUC_RUNTIME_ERROR, "var a = 1; if(\"string\") { a = 4; }"));

    // if else
    add_result(&r, run_vm_test_case(INT_VAL(1), "var a; if(1) a = 1; else a = 4;"));
    add_result(&r, run_vm_test_case(INT_VAL(4), "var a; if(0) a = 1; else a = 4;"));
    add_result(&r, run_vm_test_case(INT_VAL(1), "var a; if(true) a = 1; else a = 4;"));
    add_result(&r, run_vm_test_case(INT_VAL(4), "var a; if(false) a = 1; else a = 4;"));
    add_result(&r, run_vm_test_case(INT_VAL(1), "var a; if(1) {a = 1;} else a = 4;"));
    add_result(&r, run_vm_test_case(INT_VAL(4), "var a; if(0) a = 1; else {a = 4;}"));
    add_result(&r, run_vm_test_case(INT_VAL(1), "var a; if(true) {a = 1;} else {a = 4;}"));
    add_result(&r, run_vm_test_case(NULL_VAL, "var a; if(false) {a = 1;} else { if(false)a = 4;}"));
    add_result(&r, run_vm_test_case(INT_VAL(4), "var a; if(false) {a = 1;} else { if(2 > 1)a = 4;}"));
    add_result(&r, run_vm_test_case(INT_VAL(2), "var a; if(0) a = 1; else if(true) a = 2; else a = 4;"));
    add_result(&r, run_vm_test_case(INT_VAL(4), "var a; if(0) a = 1; else if(false) a = 2; else a = 4;"));
    add_result(&r, run_vm_test_case(INT_VAL(2), "var a; if(0) a = 1; else if(true) a = 2; else if(true) a = 4;"));
    add_result(&r, run_vm_test_case(INT_VAL(1), "var a; if(1) a = 1; else if(false) a = 2; else if(false) a = 4;"));

    // TODO: == with null
    //add_result(&r, run_vm_test_case(INT_VAL(4), "var a; if(false) {a = 1;} else { if(a == null)a = 4;}"));

    // while
    add_result(&r, run_vm_test_case(INT_VAL(100), "var a = 0; while(a<100) a = a + 1;"));
    add_result(&r, run_vm_test_case(STRING_OBJ("55555"), "var a = \"\"; var i = 0; while(i < 5) { i = i + 1;a = a + \"5\";}"));

    // for
    add_result(&r, run_vm_test_case(INT_VAL(10), "var a = 0; for(var i = 0; i < 5; i = i + 1) a = a + 2;"));
    add_result(&r, run_vm_test_case(INT_VAL(10), "var a = 0; for(;a<10;) a = a + 2;"));
    add_result(&r, run_vm_test_case(INT_VAL(0), "var a = 0; for(var i = 10;i<10;) a = 2;"));

    // local variables
    add_result(&r, run_vm_test_case(INT_VAL(0), "var a = 0; { var a = 10; }"));
    add_result(&r, run_vm_test_case(INT_VAL(0), "var a = 0; { var a = 10; { a = 11;} }"));
    add_result(&r, run_vm_test_case(INT_VAL(10), "var a = 0; { a = 10; }"));
    add_result(&r, run_vm_test_case(INT_VAL(0), "var a = 0; { var a = 10; { a = 14; }}"));
    add_result(&r, run_vm_test_case(INT_VAL(14), "var a = 0; { var a = 10;} { var b;a = 14; }"));

    
    return r;
}

TestResult run_vm_test_case(Value expecting, char *code) {
    printf("Test running expression: '%s'\n", code);
    Slice slice;
    UucResult pr = parse_code(&slice, code);
    if(pr != UUC_OK) {
        assert_fail("Unexpected parsing result.");
        return (TestResult){.result = FAIL};
    }
    VM vm = uuc_vm_init(slice);
    UucResult vm_r = uuc_vm_run(&vm);
    if(vm_r == UUC_RUNTIME_ERROR) {
        assert_fail("Unexpected runtime error.");
        return (TestResult){.result = FAIL};
    }
#if defined(UUC_LOG_TRACE)
    uuc_vm_dump(&vm);
#endif
    Value r; 
    UucString *a = uuc_create_string("a");
    int res = uuc_val_table_get(&vm.global_table, a, &r);
    if(res != 1) {
        assert_fail("Global variable 'a' not found.");
        return (TestResult){ .result = FAIL };
    }
    if(vm.value_stack.size > 0) {
        assert_fail("Expected empty stack after execution.");
        return (TestResult){ .result = FAIL };
    }
    uuc_free_string(a);
    printf(" => ");
    uuc_val_print(r);
    printf("\n");
    if(assert_value(expecting, r)) {
        return (TestResult){ .result = FAIL };
    }
    return (TestResult){ .result = PASS };
}

TestResult run_vm_error_test_case(UucResult expected_res, char *code) {
    printf("Test execute expression for error: '%s'\n", code);
    Slice slice;
    UucResult pr = parse_code(&slice, code);
    if(expected_res == UUC_COMP_ERROR && pr != UUC_COMP_ERROR) {
        assert_fail("Expected compile error.");
        return (TestResult){.result = FAIL};
    }
    if(expected_res == UUC_COMP_ERROR) return (TestResult){.result = PASS};
    VM vm = uuc_vm_init(slice);
    UucResult r = uuc_vm_run(&vm);
#if defined(UUC_LOG_TRACE)
    uuc_vm_dump(&vm);
#endif
    if(r == 0) {
        printf("\033[31mAssertion error: expected runtime error for input '%s'\033[m\n", code);
        return (TestResult){ .result = FAIL };
    }
    return (TestResult){ .result = PASS };
}

#undef INT_VAL
#undef NULL_VAL 
#undef DOUBLE_VAL
#undef BOOL_TRUE 
#undef BOOL_FALSE 
#undef STRING_OBJ 
