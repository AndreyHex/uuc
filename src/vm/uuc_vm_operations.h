#ifndef uuc_vm_operations_h
#define uuc_vm_operations_h

#include "../include/uuc_type.h"
#include "../include/vm.h"

#define CREATE_TWOARG_FUN_DEF(op_name, cmp) \
Value uuc_ ## op_name(Value right, Value left, ExeResult *r) { \
    UucType rt = right.type; \
    UucType lt = left.type;  \
    int res = -1; \
    if(rt == TYPE_INT && lt == TYPE_INT) {                \
        res = right.as.uuc_int cmp left.as.uuc_int;       \
    } else if(rt == TYPE_DOUBLE && lt == TYPE_DOUBLE) {   \
        res = right.as.uuc_double cmp left.as.uuc_double; \
    } else if(rt == TYPE_INT && lt == TYPE_DOUBLE) {      \
        res = right.as.uuc_int cmp left.as.uuc_double;    \
    } else if(rt == TYPE_DOUBLE && lt == TYPE_INT) {      \
        res = right.as.uuc_double cmp left.as.uuc_int;    \
    } else if(rt == TYPE_DOUBLE && lt == TYPE_DOUBLE) {   \
        res = right.as.uuc_double cmp left.as.uuc_double; \
    } else if(rt == TYPE_BOOL && lt == TYPE_BOOL) {       \
        res = right.as.uuc_bool cmp left.as.uuc_bool;     \
    } \
    if(res != -1) return (Value){ .type = TYPE_BOOL, .as = { .uuc_bool = res } };           \
    LOG_ERROR("Cannot compare types '%s' and '%s'.\n", uuc_type_str(rt), uuc_type_str(lt)); \
    *r = UUC_RUNTIME_ERROR; \
   return type_null(); \
}

#define CREATE_TWOARG_FUN_DECL(op_name, cmp) \
Value uuc_ ## op_name(Value right, Value left, ExeResult *r);

CREATE_TWOARG_FUN_DECL(compare_eq, ==)
CREATE_TWOARG_FUN_DECL(compare_ne, !=) 
CREATE_TWOARG_FUN_DECL(compare_gt, >) 
CREATE_TWOARG_FUN_DECL(compare_gte, >=) 
CREATE_TWOARG_FUN_DECL(compare_lt, <) 
CREATE_TWOARG_FUN_DECL(compare_lte, <=) 

#undef CREATE_TWOARG_FUN_DECL

#endif
