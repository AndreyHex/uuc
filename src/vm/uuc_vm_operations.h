#ifndef uuc_vm_operations_h
#define uuc_vm_operations_h

#include "../include/uuc_type.h"
#include "../include/uuc_vm.h"

#define CREATE_TWOARG_FUN_DEF(op_name, cmp) \
Value uuc_ ## op_name(Value left, Value right, ExeResult *r) { \
    UucType rt = right.type; \
    UucType lt = left.type;  \
    int res = -1; \
    if(lt == TYPE_INT && rt == TYPE_INT) {                \
        res = left.as.uuc_int cmp right.as.uuc_int;       \
    } else if(lt == TYPE_DOUBLE && rt == TYPE_DOUBLE) {   \
        res = left.as.uuc_double cmp right.as.uuc_double; \
    } else if(lt == TYPE_INT && rt == TYPE_DOUBLE) {      \
        res = left.as.uuc_int cmp right.as.uuc_double;    \
    } else if(lt == TYPE_DOUBLE && rt == TYPE_INT) {      \
        res = left.as.uuc_double cmp right.as.uuc_int;    \
    } else if(lt == TYPE_DOUBLE && rt == TYPE_DOUBLE) {   \
        res = left.as.uuc_double cmp right.as.uuc_double; \
    } else if(lt == TYPE_BOOL && rt == TYPE_BOOL) {       \
        res = left.as.uuc_bool cmp right.as.uuc_bool;     \
    } \
    if(res != -1) return (Value){ .type = TYPE_BOOL, .as = { .uuc_bool = res } };           \
    LOG_ERROR("Cannot compare types '%s' and '%s'.\n", uuc_type_str(lt), uuc_type_str(rt)); \
    *r = UUC_RUNTIME_ERROR; \
   return uuc_val_null(); \
}

#define CREATE_TWOARG_FUN_DECL(op_name, cmp) \
Value uuc_ ## op_name(Value left, Value right, ExeResult *r);

CREATE_TWOARG_FUN_DECL(compare_eq, ==)
CREATE_TWOARG_FUN_DECL(compare_ne, !=) 
CREATE_TWOARG_FUN_DECL(compare_gt, >) 
CREATE_TWOARG_FUN_DECL(compare_gte, >=) 
CREATE_TWOARG_FUN_DECL(compare_lt, <) 
CREATE_TWOARG_FUN_DECL(compare_lte, <=) 

#undef CREATE_TWOARG_FUN_DECL

#endif
