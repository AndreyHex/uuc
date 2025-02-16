#include "test_val_table.h"
#include "uuc_assert.h"
#include "uuc_test.h"
#include <stdint.h>

#define INT_VAL(v) (Value){ .type = TYPE_INT, .as = { .uuc_int = v } }
#define DOUBLE_VAL(v) (Value){ .type = TYPE_DOUBLE, .as = { .uuc_double = v } }
#define BOOL_TRUE (Value){ .type = TYPE_BOOL, .as = { .uuc_bool = 1 } }
#define BOOL_FALSE (Value){ .type = TYPE_BOOL, .as = { .uuc_bool = 0 } }

#define STRING_VAL(str) (Value){ .type = TYPE_OBJ, .as = { .uuc_obj = (UucObj*)uuc_create_string(str) } }
#define STRING_OBJ(str) uuc_create_string(str)

void add_one(char *b, int at) {
    if(at > 3) return;
    b[3-at]++;
    if(b[3-at] == 'Z' + 1) {
        b[3-at] = 'G';
        add_one(b, at+1);
    }
}

void run_val_table_test_insert_remove_alot_(TestResults *r) {
    UucValTable table = uuc_init_val_table(4);
    char b[4] = { 'G','G','G','\0'};
    int i = 0;
    while(1) {
        uuc_val_table_put(&table, STRING_OBJ(b), INT_VAL(i));
        i++;
        add_one(b, 0);
        if(b[0] == 'Z') break;
    }
    int a = assert_integer(i, table.size);
    printf("Test hashtable, inserted: %d -- %s\n", i, a ? "" : "passed");
    uuc_val_table_summary_dump(&table);
    if(a) {
        add_result(r, (TestResult){.result = FAIL});
        uuc_val_table_free(&table);
        return;
    }

    b[0] = 'G';b[1] = 'G';b[2] = 'G';b[3] = '\0';
    i = 0;
    int rr = 0;
    while(1) {
        rr += uuc_val_table_remove(&table, STRING_OBJ(b));
        i++;
        add_one(b, 0);
        if(b[0] == 'Z') break;
    }
    a = assert_integer(0, table.size);
    a += assert_integer(i, rr);
    printf("Test hashtable, removed: %d -- %s\n", rr, a ? "" : "passed");
    uuc_val_table_summary_dump(&table);
    add_result(r, (TestResult){.result = a ? FAIL : PASS});
    uuc_val_table_free(&table);
}

int val_table_test_fail_(TestResults *r, char *msg) {
    assert_fail(msg);
    add_result(r, (TestResult){ .result = FAIL});
    return 0;
}

int val_table_test_fail__(TestResults *r) {
    add_result(r, (TestResult){ .result = FAIL});
    return 0;
}

int run_val_table_test_put_get_(TestResults *r) {
    printf("TEST %s\n", __FUNCTION__);
    UucValTable table = uuc_init_val_table(4);
    int rr;
    int aa;
    Value v;

    // this bc need to exit and do clean up
    do { // TODO rewrite all this
    rr = uuc_val_table_put(&table, STRING_OBJ("key"), INT_VAL(33));
    if(rr > 0) {
        val_table_test_fail_(r, "put in empty table should not override");
        break;
    }

    aa = assert_integer(1, table.size);
    if(aa) break; 

    rr = uuc_val_table_get(&table, STRING_OBJ("key"), &v);
    if(rr == 0) {
        val_table_test_fail_(r, "get should return value");
        break;
    }
    aa = assert_value(INT_VAL(33), v);
    if(aa) break; 

    rr = uuc_val_table_put(&table, STRING_OBJ("keyA"), INT_VAL(99));
    if(rr > 0) {
        val_table_test_fail_(r, "put should not override");
        break;
    }

    aa = assert_integer(2, table.size);
    if(aa) break; 

    rr = uuc_val_table_get(&table, STRING_OBJ("key"), &v);
    if(rr == 0) {
        val_table_test_fail_(r, "get should return value");
        break;
    }

    aa = assert_value(INT_VAL(33), v);
    if(aa) break; 

    rr = uuc_val_table_get(&table, STRING_OBJ("keyA"), &v);
    if(rr == 0) {
        val_table_test_fail_(r, "get should return value");
        break;
    }

    aa = assert_value(INT_VAL(99), v);
    if(aa) break; 
    
    rr = uuc_val_table_put(&table, STRING_OBJ("keyA"), INT_VAL(77));
    if(rr == 0) {
        val_table_test_fail_(r, "put should override");
        break;
    }

    aa = assert_integer(2, table.size);
    if(aa) break; 

    rr = uuc_val_table_get(&table, STRING_OBJ("key"), &v);
    if(rr == 0) {
        val_table_test_fail_(r, "get should return value");
        break;
    }

    aa = assert_value(INT_VAL(33), v);
    if(aa) break;

    rr = uuc_val_table_get(&table, STRING_OBJ("keyA"), &v);
    if(rr == 0) {
        val_table_test_fail_(r, "get should return value");
        break;
    }

    } while(0);

#if defined(UUC_LOG_TRACE)
    uuc_val_table_dump(&table);
#endif
    add_result(r, (TestResult){ .result = aa ? FAIL : PASS});
    uuc_val_table_free(&table);
    return 0;
}

int run_val_table_test_put_remove_(TestResults *r) {
    printf("TEST %s\n", __FUNCTION__);
    UucValTable table = uuc_init_val_table(4);
    int rr;
    Value v;

    uuc_val_table_put(&table, STRING_OBJ("keyA"), INT_VAL(33));
    uuc_val_table_put(&table, STRING_OBJ("keyB"), INT_VAL(35));
    uuc_val_table_put(&table, STRING_OBJ("keyC"), INT_VAL(35));
    uuc_val_table_put(&table, STRING_OBJ("keyD"), INT_VAL(40));
    uuc_val_table_put(&table, STRING_OBJ("keyE"), INT_VAL(44));

    uuc_val_table_remove(&table, STRING_OBJ("keyD"));
    uuc_val_table_remove(&table, STRING_OBJ("keyC"));

    // get removed value
    int g = uuc_val_table_get(&table, STRING_OBJ("keyC"), &v);
    rr += assert_integer(0, g);

    uuc_val_table_put(&table, STRING_OBJ("keyC"), INT_VAL(999));
    uuc_val_table_get(&table, STRING_OBJ("keyC"), &v);

    rr = assert_value(INT_VAL(999), v);

#if defined(UUC_LOG_TRACE)
    uuc_val_table_dump(&table);
#endif
    rr += assert_integer(4, table.size);
    add_result(r, (TestResult){ .result = rr ? FAIL : PASS});
    uuc_val_table_free(&table);
    return 0;
}

TestResults run_val_table_tests(int argc, const char *argv[]) {
    TestResults r = init_test_results(16);
#ifndef UUC_LOG_TRACE
    run_val_table_test_insert_remove_alot_(&r);
#endif
    run_val_table_test_put_get_(&r);
    run_val_table_test_put_remove_(&r);
    return r;
}

#undef INT_VAL
#undef DOUBLE_VAL
#undef BOOL_TRUE 
#undef BOOL_FALSE 

#undef STRING_VAL
#undef STRING_OBJ
