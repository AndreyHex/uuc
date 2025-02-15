#include "test_val_table.h"

#define INT_VAL(v) (Value){ .type = TYPE_INT, .as = { .uuc_int = v } }
#define DOUBLE_VAL(v) (Value){ .type = TYPE_DOUBLE, .as = { .uuc_double = v } }
#define BOOL_TRUE (Value){ .type = TYPE_BOOL, .as = { .uuc_bool = 1 } }
#define BOOL_FALSE (Value){ .type = TYPE_BOOL, .as = { .uuc_bool = 0 } }

#define STRING_VAL(str) (Value){ .type = TYPE_OBJ, .as = { .uuc_obj = (UucObj*)uuc_create_string(str) } }
#define STRING_OBJ(str) uuc_create_string(str)

TestResults run_val_table_tests(int argc, const char *argv[]) {
    TestResults r = init_test_results(16);
    printf("Table TESTS\n");

    UucValTable table = uuc_init_val_table(4);
    UucString *key = uuc_create_string("key");
    printf("t cap is %ld\n", table.capacity);

    printf(" putting \n");
    uuc_val_table_put(&table, STRING_OBJ("a"), INT_VAL(69));
    printf(" putting \n");
    uuc_val_table_put(&table, STRING_OBJ("b"), INT_VAL(62));
    printf(" putting \n");
    uuc_val_table_put(&table, STRING_OBJ("c"), INT_VAL(59));
    printf(" putting \n");
    uuc_val_table_put(&table, STRING_OBJ("d"), INT_VAL(99));
    printf(" putting \n");
    uuc_val_table_put(&table, STRING_OBJ("e"), INT_VAL(67));
    printf(" putting \n");
    uuc_val_table_put(&table, STRING_OBJ("a"), INT_VAL(34));
    printf(" asserting \n");

    assert_integer(5, table.size);
    uuc_val_table_dump(&table);
    return r;
}

#undef INT_VAL
#undef DOUBLE_VAL
#undef BOOL_TRUE 
#undef BOOL_FALSE 

#undef STRING_VAL
#undef STRING_OBJ
