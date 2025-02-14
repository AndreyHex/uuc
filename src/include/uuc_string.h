#ifndef uuc_string_h
#define uuc_string_h

#include "uuc_type.h"

UucString *uuc_create_string(char *content);
UucString *uuc_copy_string(const char *src, size_t len);
UucString *uuc_concate_strings(UucString *left, UucString *right);
void uuc_free_string(UucString *str);

#endif
