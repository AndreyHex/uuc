#include "../include/uuc_type.h"
#include <stddef.h>

const char *uuc_type_strings_[] = {
    "int",        // TYPE_INT
    "double",     // TYPE_DOUBLE
    "boolean",    // TYPE_BOOL
    "null",       // TYPE_NULL
    "obj",        // TYPE_OBJ
};

const char *uuc_type_str(UucType type) {
    printf("tpye str for %d \n", type);
    return uuc_type_strings_[type];
}
