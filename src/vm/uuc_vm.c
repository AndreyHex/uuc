#include "../include/uuc_string.h"
#include "../include/uuc_bytecode.h"
#include "../include/uuc_vm.h"
#include "../include/uuc_type_print.h"
#include <stdint.h>
#include <stdio.h>

#include "uuc_vm_operations.h"

UucResult vm_tick(VM *vm);
void uuc_vm_advance(UucCallFrame *frame);
void uuc_vm_advance_for(UucCallFrame *frame, int offset);
UucCallFrame uuc_create_call_frame(VM *vm, UucFunction *function);
int uuc_type_check(Value val, UucType type, const char *msg);
int uuc_null_check(Value *val);
int uuc_zero_division_check(Value divisor);
Value uuc_op_add(Value left, Value right, UucResult *r);
Value uuc_op_substract(Value left, Value right, UucResult *r);
Value uuc_op_divide(Value divident, Value divisor, UucResult *r);
Value uuc_op_multiply(Value left, Value right, UucResult *r);
Value uuc_compare_eq(Value left, Value right, UucResult *r);

void uuc_vm_dump(VM *vm);

VM uuc_vm_init(UucFunction *main) {
    VM vm = {
        .main = main,
        .value_stack = stack_init(32),
        .global_table = uuc_init_val_table(16),
        .frames_size = 1,
    };
    vm.call_frames[0] = uuc_create_call_frame(&vm, main);
    return vm;
}

UucResult uuc_vm_run(VM *vm) { 
    UucResult r = UUC_OK;
    while(r == UUC_OK) {
#if defined (UUC_LOG_TRACE)
//    list_print(&vm->slice.constants);
//    uuc_vm_dump(vm);
#endif
        r = vm_tick(vm);
    }
    return r;
}

void uuc_vm_advance(UucCallFrame *frame) {
    frame->ip++;
    frame->ii++;
}

void uuc_vm_advance_for(UucCallFrame *frame, int offset) {
    frame->ip += offset;
    frame->ii += offset;
}

#define execute_operation(fun_name) \
do { \
Value b = stack_pop(stack); \
if(uuc_null_check(&b)) return UUC_RUNTIME_ERROR; \
Value a = stack_pop(stack); \
if(uuc_null_check(&a)) return UUC_RUNTIME_ERROR; \
stack_push(stack, fun_name(a, b, &r)); } while(0);

UucResult vm_tick(VM *vm) {
    UucCallFrame *frame = &vm->call_frames[vm->frames_size - 1];
    uint32_t stack_offset = frame->stack_offset;
    uint8_t ip = *frame->ip;
    Values *stack = &vm->value_stack;
    Values *constants = &frame->function->bytecode.constants;
    Values *names = &frame->function->bytecode.names;
    UucValTable *globals = &vm->global_table;

    LOG_TRACE("VM current operation: <%s> %ld:%s \n", frame->function->name->content, frame->ii, opcode_name(ip));
    //stack_print(stack);
    //list_print(constants);
    //list_print(names);

    UucResult r = UUC_OK;
    switch(ip) {
        case OP_CONSTANT: {
            uuc_vm_advance(frame);
            uint8_t index = *frame->ip;
            stack_push(stack, list_get(constants, index));
            break;
        }
        case OP_CONSTANT_16: {
            uint8_t left = 000; // next index 
            uint8_t right = 000; // next next index byte
            uint16_t index = right | (left << 8);
            stack_push(stack, list_get(constants, index));
            break;
        }
        case OP_DEFINE_GLOBAL: {
            uuc_vm_advance(frame);
            uint8_t index = *frame->ip;
            UucString *name = (UucString *)list_get(names, index).as.uuc_obj;
            Value val = stack_peek(stack);
            uuc_val_table_put(globals, name, val);
            stack_pop(stack);
            break;
        }
        case OP_GET_GLOBAL: {
            uuc_vm_advance(frame);
            uint8_t index = *frame->ip;
            UucString *name = (UucString *)list_get(names, index).as.uuc_obj;
            Value val;
            int r = uuc_val_table_get(globals, name, &val);
            if(r == 0) {
                LOG_ERROR("Undefined variable '%s'\n", name->content);
                return UUC_RUNTIME_ERROR;
            }
            stack_push(stack, val);
            break;
        }
        case OP_SET_GLOBAL: {
            uuc_vm_advance(frame);
            uint8_t index = *frame->ip;
            UucString *name = (UucString *)list_get(names, index).as.uuc_obj;
            Value val;
            int r = uuc_val_table_get(globals, name, &val);
            if(r == 0) {
                LOG_ERROR("Undefined variable '%s'\n", name->content);
                return UUC_RUNTIME_ERROR;
            }
            uuc_val_table_put(globals, name, stack_pop(stack));
            break;
        }
        case OP_GET_LOCAL: {
            uuc_vm_advance(frame);
            uint8_t index = *frame->ip;
            stack_push(stack, stack_get(stack, stack_offset + index));
            break;
        }
        case OP_SET_LOCAL: {
            uuc_vm_advance(frame);
            uint8_t index = *frame->ip;
            stack_set(stack, stack_offset + index, stack_pop(stack));
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
            stack_push(stack, uuc_val_int(-(v.as.uuc_int)));
            break;
        }
        case OP_ADD: execute_operation(uuc_op_add) break;
        case OP_SUBSTRACT: execute_operation(uuc_op_substract) break;
        case OP_MULTIPLY: execute_operation(uuc_op_multiply) break;
        case OP_DIVIDE: execute_operation(uuc_op_divide) break;
        case OP_TRUE: stack_push(stack, uuc_val_bool(1)); break;
        case OP_FALSE: stack_push(stack, uuc_val_bool(0)); break;
        case OP_NULL: stack_push(stack, uuc_val_null()); break;

        case OP_EQ: execute_operation(uuc_compare_eq) break;
        case OP_NE: execute_operation(uuc_compare_ne) break;
        case OP_GT: execute_operation(uuc_compare_gt) break;
        case OP_GTE: execute_operation(uuc_compare_gte) break;
        case OP_LT: execute_operation(uuc_compare_lt) break;
        case OP_LTE: execute_operation(uuc_compare_lte) break;

        case OP_JUMP_IF_FALSE: {
            Value v = stack_pop(stack);
            if(uuc_null_check(&v)) return UUC_RUNTIME_ERROR;
            int r = 0;
            switch (v.type) {
                case TYPE_BOOL: r = v.as.uuc_bool; break;
                case TYPE_INT: r = v.as.uuc_int; break;
                case TYPE_DOUBLE: r = v.as.uuc_int; break;
                default: {
                    LOG_ERROR("Type '%s' cannot be casted to boolean\n", uuc_type_str(v.type));
                    return UUC_RUNTIME_ERROR;
                }
            }
            if(r) {
                // skip jump offset
                uuc_vm_advance(frame);
                uuc_vm_advance(frame);
            } else {
                uuc_vm_advance(frame);
                uint8_t left = *frame->ip;
                uuc_vm_advance(frame);
                uint8_t right = *frame->ip;
                uint16_t jump_offset = right | (left << 8);
                uuc_vm_advance_for(frame, jump_offset);
            }
            break;
        }
        case OP_JUMP_BACK:
        case OP_JUMP: {
            uuc_vm_advance(frame);
            uint8_t left = *frame->ip;
            uuc_vm_advance(frame);
            uint8_t right = *frame->ip;
            uint16_t jump_offset = right | (left << 8);
            if(ip == OP_JUMP) uuc_vm_advance_for(frame, jump_offset);
            else if(ip == OP_JUMP_BACK) uuc_vm_advance_for(frame, -jump_offset);
            break;
        }
        case OP_CALL: {
            uuc_vm_advance(frame);
            uint8_t arity = *frame->ip;
            Value fn = stack_pop(stack);
            if(fn.type != TYPE_OBJ || fn.as.uuc_obj->type != OBJ_FUNCTION) {
                LOG_ERROR("Cannot call non function\n");
                return UUC_RUNTIME_ERROR;
            }
            vm->call_frames[vm->frames_size++] = uuc_create_call_frame(vm, (UucFunction*)fn.as.uuc_obj);
            break;
        }
        case OP_RETURN: {
            Value ret = stack_pop(stack);
            uint32_t locals = stack->size - stack_offset;
            for(int i = 0; i < locals; i++) {
                stack_pop(stack);
            }
            vm->frames_size--;
            stack_push(stack, ret);
            break;
        }
        case OP_POP: {
            stack_pop(stack);
            break;
        }
        default: {
            LOG_ERROR("UNKNOWN OP CODE\n");
            return UUC_RUNTIME_ERROR;
        }
    }
    uuc_vm_advance(frame);
    // if ok and in main and last instruction return END
    if(r == UUC_OK && 
       vm->frames_size == 1 &&
       frame->ii >= frame->function->bytecode.size) return UUC_OK_END;
    return r;
}
#undef execute_operation

UucCallFrame uuc_create_call_frame(VM *vm, UucFunction *function) {
    return (UucCallFrame){
        .function = function,
        .stack_offset = vm->value_stack.size - function->arity,
        .ip = function->bytecode.codes,
        .ii = 0,
    };
}

// plz send help
Value uuc_op_add(Value left, Value right, UucResult *r) {
    if(left.type == TYPE_OBJ && left.as.uuc_obj->type == OBJ_STRING &&
       right.type == TYPE_OBJ && right.as.uuc_obj->type == OBJ_STRING) {
        UucString *str = uuc_concate_strings((UucString*)left.as.uuc_obj, 
                                             (UucString*)right.as.uuc_obj);
        return (Value){ .type = TYPE_OBJ, .as = { .uuc_obj = (UucObj*)str }};
    }
    if((left.type != TYPE_INT && left.type != TYPE_DOUBLE) ||
       (right.type != TYPE_INT && right.type != TYPE_DOUBLE)) {
        LOG_ERROR("Cannot add type '%s' to type '%s'.\n", uuc_type_str(right.type), uuc_type_str(left.type));
        *r = UUC_RUNTIME_ERROR;
        return uuc_val_null();
    }
    if(left.type == TYPE_INT && right.type == TYPE_INT) {
        long r = ((double)left.as.uuc_int) + (double)right.as.uuc_int;
        return (Value){ .type = TYPE_INT, .as = { .uuc_int = r } };
    } else if(left.type == TYPE_DOUBLE && right.type == TYPE_DOUBLE) {
        double r = left.as.uuc_double + right.as.uuc_double;
        return (Value){ .type = TYPE_DOUBLE, .as = { .uuc_double = r } };
    } else if(left.type == TYPE_INT && right.type == TYPE_DOUBLE) {
        double r = ((double)left.as.uuc_int) + right.as.uuc_double;
        return (Value){ .type = TYPE_DOUBLE, .as = { .uuc_double = r } };
    } else if(left.type == TYPE_DOUBLE && right.type == TYPE_INT) {
        double r = left.as.uuc_double + (double)right.as.uuc_int;
        return (Value){ .type = TYPE_DOUBLE, .as = { .uuc_double = r } };
    } else if(left.type == TYPE_DOUBLE && right.type == TYPE_DOUBLE) {
        double r = left.as.uuc_double + right.as.uuc_double;
        return (Value){ .type = TYPE_DOUBLE, .as = { .uuc_double = r } };
    } 
    return uuc_val_null();
}

Value uuc_op_substract(Value left, Value right, UucResult *r) {
    if((left.type != TYPE_INT && left.type != TYPE_DOUBLE) ||
       (right.type != TYPE_INT && right.type != TYPE_DOUBLE)) {
        LOG_ERROR("Cannot substract type '%s' from type '%s'.\n", uuc_type_str(right.type), uuc_type_str(left.type));
        *r = UUC_RUNTIME_ERROR;
        return uuc_val_null();
    }
    if(left.type == TYPE_INT && right.type == TYPE_INT) {
        long r = ((double)left.as.uuc_int) - (double)right.as.uuc_int;
        return (Value){ .type = TYPE_INT, .as = { .uuc_int = r } };
    } else if(left.type == TYPE_DOUBLE && right.type == TYPE_DOUBLE) {
        double r = left.as.uuc_double - right.as.uuc_double;
        return (Value){ .type = TYPE_DOUBLE, .as = { .uuc_double = r } };
    } else if(left.type == TYPE_INT && right.type == TYPE_DOUBLE) {
        double r = ((double)left.as.uuc_int) - right.as.uuc_double;
        return (Value){ .type = TYPE_DOUBLE, .as = { .uuc_double = r } };
    } else if(left.type == TYPE_DOUBLE && right.type == TYPE_INT) {
        double r = left.as.uuc_double - (double)right.as.uuc_int;
        return (Value){ .type = TYPE_DOUBLE, .as = { .uuc_double = r } };
    } else if(left.type == TYPE_DOUBLE && right.type == TYPE_DOUBLE) {
        double r = left.as.uuc_double - right.as.uuc_double;
        return (Value){ .type = TYPE_DOUBLE, .as = { .uuc_double = r } };
    }
    return uuc_val_null();
}

Value uuc_op_multiply(Value left, Value right, UucResult *r) {
    if((left.type != TYPE_INT && left.type != TYPE_DOUBLE) ||
       (right.type != TYPE_INT && right.type != TYPE_DOUBLE)) {
        LOG_ERROR("Type '%s' cannot be multiplied by type '%s'.\n", uuc_type_str(left.type), uuc_type_str(right.type));
        *r = UUC_RUNTIME_ERROR;
        return uuc_val_null();
    }
    if(left.type == TYPE_INT && right.type == TYPE_INT) {
        long r = ((double)left.as.uuc_int) * (double)right.as.uuc_int;
        return (Value){ .type = TYPE_INT, .as = { .uuc_int = r } };
    } else if(left.type == TYPE_DOUBLE && right.type == TYPE_DOUBLE) {
        double r = left.as.uuc_double * right.as.uuc_double;
        return (Value){ .type = TYPE_DOUBLE, .as = { .uuc_double = r } };
    } else if(left.type == TYPE_INT && right.type == TYPE_DOUBLE) {
        double r = ((double)left.as.uuc_int) * right.as.uuc_double;
        return (Value){ .type = TYPE_DOUBLE, .as = { .uuc_double = r } };
    } else if(left.type == TYPE_DOUBLE && right.type == TYPE_INT) {
        double r = left.as.uuc_double * (double)right.as.uuc_int;
        return (Value){ .type = TYPE_DOUBLE, .as = { .uuc_double = r } };
    } else if(left.type == TYPE_DOUBLE && right.type == TYPE_DOUBLE) {
        double r = left.as.uuc_double * right.as.uuc_double;
        return (Value){ .type = TYPE_DOUBLE, .as = { .uuc_double = r } };
    }
    return uuc_val_null();
}

Value uuc_op_divide(Value divident, Value divisor, UucResult *r) {
    if((divident.type != TYPE_INT && divident.type != TYPE_DOUBLE) ||
       (divisor.type != TYPE_INT && divisor.type != TYPE_DOUBLE)) {
        LOG_ERROR("Type '%s' cannot be divided by type '%s'.\n", uuc_type_str(divident.type), uuc_type_str(divisor.type));
        *r = UUC_RUNTIME_ERROR;
        return uuc_val_null();
    }
    if(uuc_zero_division_check(divisor)) {
        *r = UUC_RUNTIME_ERROR;
        return uuc_val_null();
    }
    if(divident.type == TYPE_INT && divisor.type == TYPE_INT) {
        long r = divident.as.uuc_int / divisor.as.uuc_int;
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
    return uuc_val_null();
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

void uuc_free_vm(VM *vm) {
    stack_free(&vm->value_stack);
    uuc_free_function(vm->main);
    uuc_val_table_free(&vm->global_table);
}

void uuc_vm_dump_bytecode(VM *vm, UucFunction *fn) {
    printf("Function <%s>:\n", fn->name->content);
    Slice *slice = &fn->bytecode;
    int ii = 0;
    for(int i = 0; i < slice->size; i++) {
        uint8_t code = slice->codes[i];
        if(i == ii) printf("> %4d | ", i);
        else printf("  %4d | ", i);
        if(code == OP_CONSTANT) {
            uint8_t index = slice->codes[i + 1];
            Value v = slice->constants.head[index];
            printf("%3d:%s = %d:", code, opcode_name(code), index);
            uuc_val_print(v);
            i++;
        } else if(code == OP_CONSTANT_16) {
            LOG_ERROR("Unsupported index constant length: 16!\n");
        } else if(code == OP_DEFINE_GLOBAL) {
            printf("%3d:%s  ", code, opcode_name(code));
            uint8_t index = slice->codes[i + 1];
            Value name_v = slice->names.head[index];
            UucString *name = (UucString*)name_v.as.uuc_obj;
            printf("%s = ", name->content);
            Value val;
            int r = uuc_val_table_get(&vm->global_table, name, &val);
            if(r) uuc_val_print(val);
            else uuc_val_print(uuc_val_null());
            i++;
        } else if(code == OP_GET_GLOBAL) {
            uint8_t index = slice->codes[i + 1];
            printf("%3d:%s index:%d", code, opcode_name(code), index);
            i++;
        } else if(code == OP_SET_GLOBAL) {
            printf("%3d:%s", code, opcode_name(code));
            i++;
        } else if(code == OP_SET_LOCAL || code == OP_GET_LOCAL) {
            uint8_t index = slice->codes[i + 1];
            printf("%3d:%s stack_index:%d", code, opcode_name(code), index);
            i++;
        } else if(code == OP_JUMP || code == OP_JUMP_BACK || code == OP_JUMP_IF_FALSE) {
            uint8_t left = slice->codes[i + 1];
            uint8_t right = slice->codes[i + 2];
            uint16_t jump_offset = right | (left << 8);
            i += 2;
            int offset = code == OP_JUMP_BACK ? -jump_offset : jump_offset;
            printf("%3d:%s to %d", code, opcode_name(code), i + offset + 1);
        } else if(code == OP_CALL) {
            uint8_t args = slice->codes[i + 1];
            printf("%3d:%s args:%d", code, opcode_name(code), args);
            i++;
        } else {
            printf("%3d:%s", code, opcode_name(code));
        }
        printf("\n");
    }
    for(int i = 0; i < slice->constants.size; i++) {
        Value v = slice->constants.head[i];
        if(v.type == TYPE_OBJ && v.as.uuc_obj->type == OBJ_FUNCTION) {
            uuc_vm_dump_bytecode(vm, (UucFunction *)v.as.uuc_obj);
        }
    }
}

void uuc_vm_dump(VM *vm) {
    Slice *slice = &vm->main->bytecode;
    printf("====== Bytecode slice dump ======\n");
    printf(" Bytes: %d\n", slice->size);
    printf(" Instructions: %d\n", slice->size - slice->constants.size);
    printf(" Constants:\n  ");
    list_print(&slice->constants);
    printf(" Names:\n  ");
    list_print(&slice->names);
    printf("============ Globals ============\n");
    uuc_val_table_dump(&vm->global_table);
    printf("============ Content ============\n");
    uuc_vm_dump_bytecode(vm, vm->main);
    printf("=================================\n");
}
