#include "../include/uuc_s_notation.h"
#include "../include/uuc_bytecode.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

const char *opcode_op_char(OpCode opcode);

// create string epression in s-notation (polish) for testing puproses
// expecting single expression in slice
void slice_s_notation(UucFunction *function, char *buf, uint32_t buf_s) {
    Slice *slice = &function->bytecode;
    char *bb = malloc(buf_s * sizeof(char));
    int s_cap = 32;
    int s_size = 0;
    char *stack = malloc(buf_s * sizeof(char) * s_cap);

    for(int i = 0; i < slice->size; i++) {
        if(s_size == s_cap) {
            s_cap *= 2;
            stack = realloc(stack, s_cap * buf_s * sizeof(char));
        }
        OpCode code = slice->codes[i];
        if(code == OP_CONSTANT) {
            i++;
            int index = slice->codes[i];
            Value v = slice->constants.head[index];
            switch(v.type) {
                case TYPE_INT: sprintf(&stack[s_size * buf_s], "%ld", v.as.uuc_int); break;
                case TYPE_DOUBLE: sprintf(&stack[s_size * buf_s], "%.2f", v.as.uuc_double); break;
                case TYPE_BOOL: sprintf(&stack[s_size * buf_s], "%s", v.as.uuc_bool ? "true" : "false"); break;
                case TYPE_OBJ: sprintf(&stack[s_size * buf_s], "\"%s\"", ((UucString*)v.as.uuc_obj)->content); break;
                default: break;
            }
            s_size++;
        } else if(code == OP_DEFINE_GLOBAL || code == OP_SET_GLOBAL) {
            i++;
            int index = slice->codes[i];
            UucString *name = (UucString *)slice->names.head[index].as.uuc_obj;
            s_size--;
            char *a = &stack[s_size * buf_s];
            if(code == OP_DEFINE_GLOBAL) sprintf(bb, "( var %s %s )", name->content, a);
            else sprintf(bb, "( = %s %s )", name->content, a);
            memcpy(&stack[s_size * buf_s], bb, buf_s);
            s_size++;
        } else if(code == OP_GET_GLOBAL) {
            i++;
            int index = slice->codes[i];
            UucString *name = (UucString *)slice->names.head[index].as.uuc_obj;
            sprintf(&stack[s_size * buf_s], "%s", name->content);
            s_size++;
        } else if(code == OP_TRUE || code == OP_FALSE) {
            sprintf(&stack[s_size * buf_s], "%s", code == OP_TRUE ? "true" : "false");
            s_size++;
        } else if(code == OP_NULL) {
            sprintf(&stack[s_size * buf_s], "%s", "null");
            s_size++;
        } else if((code >= OP_ADD && code <= OP_LTE) || code == OP_SET_GLOBAL) {
            s_size--;
            char *a = &stack[s_size * buf_s];
            s_size--;
            char *b = &stack[s_size * buf_s];
            sprintf(bb, "( %s %s %s )", opcode_op_char(code), b, a);
            memcpy(&stack[s_size * buf_s], bb, buf_s);
            s_size++;
        } else if (code >= OP_NEGATE && code <= OP_NOT) {
            s_size--;
            char *a = &stack[s_size * buf_s];
            sprintf(bb, "( %s %s )", opcode_op_char(code), a);
            memcpy(&stack[s_size * buf_s], bb, buf_s);
            s_size++;
        } else {
            LOG_ERROR("Unknown op_code: '%s'\n", opcode_name(code));
        }
    }
    sprintf(buf, "%s", stack);
    free(stack);
}

const char *opcode_op_char(OpCode opcode) {
    switch(opcode) {
        case OP_SET_GLOBAL: return "=";
        case OP_ADD: return "+";
        case OP_SUBSTRACT: return "-";
        case OP_MULTIPLY: return "*";
        case OP_DIVIDE: return "/";
        case OP_NOT: return "!";
        case OP_NEGATE: return "-";
        case OP_EQ: return "==";
        case OP_NE: return "!=";
        case OP_GT: return ">";
        case OP_GTE: return ">=";
        case OP_LT: return "<";
        case OP_LTE: return "<=";
        default: return "e";
    }
}
