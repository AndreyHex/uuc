#include "../include/uuc_type.h"

const char *uuc_type_strings_[4] = {
    "TYPE_INT",
    "TYPE_DOUBLE",
    "TYPE_BOOL",
    "TYPE_NULL",
};

const char *uuc_type_str(UucType type) {
    return uuc_type_strings_[type];
}
