#include "../include/bytecode.h"
#include "../include/vm.h"
#include <stdint.h>
#include <stdio.h>

ExeResult vm_tick(VM *vm);
void vm_advance(VM *vm);
int uuc_type_check(Value val, UucType type, const char *msg);
int uuc_null_check(Value *val);
int uuc_zero_division_check(Value divisor);
Value uuc_op_add(Value right, Value left, ExeResult *r);
Value uuc_op_substract(Value right, Value left, ExeResult *r);
Value uuc_op_divide(Value divident, Value divisor, ExeResult *r);
Value uuc_op_multiply(Value right, Value left, ExeResult *r);

VM vm_init(Slice slice) {
    VM vm = {
        .slice = slice,
        .ip = slice.codes,
        .ii = 0,
        .value_stack = stack_init(32),
    };
    return vm;
}

ExeResult vm_run(VM *vm) { 
    while(vm->ii < vm->slice.size) {
        ExeResult r = vm_tick(vm);
        if(r) return r;
        vm_advance(vm);
    }
    return UUC_OK;
}

void vm_advance(VM *vm) {
    vm->ip++;
    vm->ii++;
}

ExeResult vm_tick(VM *vm) {
    uint8_t ip = *vm->ip;
    Values *stack = &vm->value_stack;
    Values *constants = &vm->slice.constants;
    ExeResult r = UUC_OK;
    switch(ip) {
        case OP_CONSTANT: {
            vm_advance(vm);
            uint8_t index = *vm->ip;
            stack_push(stack, list_get(constants, index));
            break;
        }
        case OP_CONSTANT_16: {
            uint8_t left = 000; // next index 
            uint8_t right = 000; // next next index byte
            uint16_t index = left | (right << 8);
            stack_push(stack, list_get(constants, index));
            break;
        }
        case OP_NOT: {
            Value v = stack_pop(stack);
            if(uuc_null_check(&v)) return UUC_RUNTIME_ERROR;
            if(uuc_type_check(v, TYPE_BOOL, "Cannot apply logical not to non bool type.")) return UUC_RUNTIME_ERROR;
            v.as.uuc_bool = !v.as.uuc_bool;
            stack_push(stack, v);
            break;
        }
        case OP_NEGATE: {
            Value v = stack_pop(stack);
            if(uuc_null_check(&v)) return UUC_RUNTIME_ERROR;
            if(uuc_type_check(v, TYPE_INT, "Cannot negate non number type.")) return UUC_RUNTIME_ERROR;
            stack_push(stack, type_int(-(v.as.uuc_int)));
            break;
        }
        case OP_ADD: {
            Value b = stack_pop(stack);
            if(uuc_null_check(&b)) return UUC_RUNTIME_ERROR;
            Value a = stack_pop(stack);
            if(uuc_null_check(&a)) return UUC_RUNTIME_ERROR;
            stack_push(stack, uuc_op_add(a, b, &r));
            break;
        }
        case OP_SUBSTRACT: {
            Value b = stack_pop(stack);
            if(uuc_null_check(&b)) return UUC_RUNTIME_ERROR;
            Value a = stack_pop(stack);
            if(uuc_null_check(&a)) return UUC_RUNTIME_ERROR;
            stack_push(stack, uuc_op_substract(a, b, &r));
            break;
        }
        case OP_MULTIPLY: {
            Value b = stack_pop(stack);
            if(uuc_null_check(&b)) return UUC_RUNTIME_ERROR;
            Value a = stack_pop(stack);
            if(uuc_null_check(&a)) return UUC_RUNTIME_ERROR;
            stack_push(stack, uuc_op_multiply(a, b, &r));
            break;
        }
        case OP_DIVIDE: {
            Value b = stack_pop(stack);
            if(uuc_null_check(&b)) return UUC_RUNTIME_ERROR;
            Value a = stack_pop(stack);
            if(uuc_null_check(&a)) return UUC_RUNTIME_ERROR;
            stack_push(stack, uuc_op_divide(a, b, &r));
            break;
        }
        case OP_RETURN: {
            printf("OP_RETURN\n");
            // printf("%f\n", stack_pop(stack));
            break;
        }
        default: printf("DEFAULT\n");
    }
    #if defined (UUC_LOG_TRACE)
    stack_print(&vm->value_stack);
    #endif
    return r;
}

// plz send help
Value uuc_op_add(Value right, Value left, ExeResult *r) {
    if((right.type != TYPE_INT && right.type != TYPE_DOUBLE) ||
       (left.type != TYPE_INT && left.type != TYPE_DOUBLE)) {
        LOG_ERROR("Cannot add type '%s' to type '%s'.\n", uuc_type_str(left.type), uuc_type_str(right.type));
        *r = UUC_RUNTIME_ERROR;
        return type_null();
    }
    if(right.type == TYPE_INT && left.type == TYPE_INT) {
        long r = ((double)right.as.uuc_int) + (double)left.as.uuc_int;
        return (Value){ .type = TYPE_INT, .as = { .uuc_int = r } };
    } else if(right.type == TYPE_DOUBLE && left.type == TYPE_DOUBLE) {
        double r = right.as.uuc_double + left.as.uuc_double;
        return (Value){ .type = TYPE_DOUBLE, .as = { .uuc_double = r } };
    } else if(right.type == TYPE_INT && left.type == TYPE_DOUBLE) {
        double r = ((double)right.as.uuc_int) + left.as.uuc_double;
        return (Value){ .type = TYPE_DOUBLE, .as = { .uuc_double = r } };
    } else if(right.type == TYPE_DOUBLE && left.type == TYPE_INT) {
        double r = right.as.uuc_double + (double)left.as.uuc_int;
        return (Value){ .type = TYPE_DOUBLE, .as = { .uuc_double = r } };
    } else if(right.type == TYPE_DOUBLE && left.type == TYPE_DOUBLE) {
        double r = right.as.uuc_double + left.as.uuc_double;
        return (Value){ .type = TYPE_DOUBLE, .as = { .uuc_double = r } };
    }
    return type_null();
}

Value uuc_op_substract(Value right, Value left, ExeResult *r) {
    if((right.type != TYPE_INT && right.type != TYPE_DOUBLE) ||
       (left.type != TYPE_INT && left.type != TYPE_DOUBLE)) {
        LOG_ERROR("Cannot substract type '%s' from type '%s'.\n", uuc_type_str(left.type), uuc_type_str(right.type));
        *r = UUC_RUNTIME_ERROR;
        return type_null();
    }
    if(right.type == TYPE_INT && left.type == TYPE_INT) {
        long r = ((double)right.as.uuc_int) - (double)left.as.uuc_int;
        return (Value){ .type = TYPE_INT, .as = { .uuc_int = r } };
    } else if(right.type == TYPE_DOUBLE && left.type == TYPE_DOUBLE) {
        double r = right.as.uuc_double - left.as.uuc_double;
        return (Value){ .type = TYPE_DOUBLE, .as = { .uuc_double = r } };
    } else if(right.type == TYPE_INT && left.type == TYPE_DOUBLE) {
        double r = ((double)right.as.uuc_int) - left.as.uuc_double;
        return (Value){ .type = TYPE_DOUBLE, .as = { .uuc_double = r } };
    } else if(right.type == TYPE_DOUBLE && left.type == TYPE_INT) {
        double r = right.as.uuc_double - (double)left.as.uuc_int;
        return (Value){ .type = TYPE_DOUBLE, .as = { .uuc_double = r } };
    } else if(right.type == TYPE_DOUBLE && left.type == TYPE_DOUBLE) {
        double r = right.as.uuc_double - left.as.uuc_double;
        return (Value){ .type = TYPE_DOUBLE, .as = { .uuc_double = r } };
    }
    return type_null();
}

Value uuc_op_multiply(Value right, Value left, ExeResult *r) {
    if((right.type != TYPE_INT && right.type != TYPE_DOUBLE) ||
       (left.type != TYPE_INT && left.type != TYPE_DOUBLE)) {
        LOG_ERROR("Type '%s' cannot be multiplied by type '%s'.\n", uuc_type_str(right.type), uuc_type_str(left.type));
        *r = UUC_RUNTIME_ERROR;
        return type_null();
    }
    if(right.type == TYPE_INT && left.type == TYPE_INT) {
        long r = ((double)right.as.uuc_int) * (double)left.as.uuc_int;
        return (Value){ .type = TYPE_INT, .as = { .uuc_int = r } };
    } else if(right.type == TYPE_DOUBLE && left.type == TYPE_DOUBLE) {
        double r = right.as.uuc_double * left.as.uuc_double;
        return (Value){ .type = TYPE_DOUBLE, .as = { .uuc_double = r } };
    } else if(right.type == TYPE_INT && left.type == TYPE_DOUBLE) {
        double r = ((double)right.as.uuc_int) * left.as.uuc_double;
        return (Value){ .type = TYPE_DOUBLE, .as = { .uuc_double = r } };
    } else if(right.type == TYPE_DOUBLE && left.type == TYPE_INT) {
        double r = right.as.uuc_double * (double)left.as.uuc_int;
        return (Value){ .type = TYPE_DOUBLE, .as = { .uuc_double = r } };
    } else if(right.type == TYPE_DOUBLE && left.type == TYPE_DOUBLE) {
        double r = right.as.uuc_double * left.as.uuc_double;
        return (Value){ .type = TYPE_DOUBLE, .as = { .uuc_double = r } };
    }
    return type_null();
}

Value uuc_op_divide(Value divident, Value divisor, ExeResult *r) {
    if((divident.type != TYPE_INT && divident.type != TYPE_DOUBLE) ||
       (divisor.type != TYPE_INT && divisor.type != TYPE_DOUBLE)) {
        LOG_ERROR("Type '%s' cannot be divided by type '%s'.\n", uuc_type_str(divident.type), uuc_type_str(divisor.type));
        *r = UUC_RUNTIME_ERROR;
        return type_null();
    }
    if(uuc_zero_division_check(divisor)) {
        *r = UUC_RUNTIME_ERROR;
        return type_null();
    }
    if(divident.type == TYPE_INT && divisor.type == TYPE_INT) {
        long r = ((double)divident.as.uuc_int) / (double)divisor.as.uuc_int;
        return (Value){ .type = TYPE_INT, .as = { .uuc_int = r } };
    } else if(divident.type == TYPE_DOUBLE && divisor.type == TYPE_DOUBLE) {
        double r = divident.as.uuc_double / divisor.as.uuc_double;
        return (Value){ .type = TYPE_DOUBLE, .as = { .uuc_double = r } };
    } else if(divident.type == TYPE_INT && divisor.type == TYPE_DOUBLE) {
        double r = ((double)divident.as.uuc_int) / divisor.as.uuc_double;
        return (Value){ .type = TYPE_DOUBLE, .as = { .uuc_double = r } };
    } else if(divident.type == TYPE_DOUBLE && divisor.type == TYPE_INT) {
        double r = divident.as.uuc_double / (double)divisor.as.uuc_int;
        return (Value){ .type = TYPE_DOUBLE, .as = { .uuc_double = r } };
    } else if(divident.type == TYPE_DOUBLE && divisor.type == TYPE_DOUBLE) {
        double r = divident.as.uuc_double / divisor.as.uuc_double;
        return (Value){ .type = TYPE_DOUBLE, .as = { .uuc_double = r } };
    }
    return type_null();
}

int uuc_null_check(Value *val) {
    if(val->type == TYPE_NULL) {
        LOG_ERROR("Null pointer exception.\n");
        return 1;
    }
    return 0;
}

int uuc_zero_division_check(Value divisor) {
    if((divisor.type == TYPE_INT && divisor.as.uuc_int == 0) ||
       (divisor.type == TYPE_DOUBLE && divisor.as.uuc_double == 0)) {
        LOG_ERROR("Cannot divide by zero.\n");
        return 1;
    }
    return 0;
}

int uuc_type_check(Value val, UucType type, const char *msg) {
    if(val.type != type) {
        LOG_TRACE("%s\n", msg);
        return 1;
    }
    return 0;
}
