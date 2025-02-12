#include "uuc_vm_operations.h"

CREATE_TWOARG_FUN_DEF(compare_eq, ==)
CREATE_TWOARG_FUN_DEF(compare_ne, !=) 
CREATE_TWOARG_FUN_DEF(compare_gt, >) 
CREATE_TWOARG_FUN_DEF(compare_gte, >=) 
CREATE_TWOARG_FUN_DEF(compare_lt, <) 
CREATE_TWOARG_FUN_DEF(compare_lte, <=) 

#undef CREATE_TWOARG_FUN_DEF
