#include "../include/uuc_type.h"

const char *uuc_type_strings_[4] = {
    "int",        // TYPE_INT
    "double",     // TYPE_DOUBLE
    "boolean",    // TYPE_BOOL
    "null",       // TYPE_NULL
};

const char *uuc_type_str(UucType type) {
    return uuc_type_strings_[type];
}
