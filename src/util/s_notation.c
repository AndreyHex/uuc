#include "../include/s_notation.h"
#include "../include/bytecode.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

char opcode_op_char(OpCode opcode);

// create string epression in s-notation (polish) for testing puproses
// expecting single expression in slice
void slice_s_notation(Slice *slice, char *buf, uint32_t buf_s) {
    char *bb = malloc(buf_s * sizeof(char));
    int s_cap = 32;
    int s_size = 0;
    char *stack = malloc(buf_s * sizeof(char) * s_cap);

    for(int i = 0; i < slice->size; i++) {
        OpCode code = slice->codes[i];
        if(code == OP_CONSTANT) {
            i++;
            int index = slice->codes[i];
            if(s_size == s_cap) {
                s_cap *= 2;
                stack = realloc(stack, s_cap * buf_s * sizeof(char));
            }
            Value v = slice->constants.head[index];
            switch(v.type) {
                case TYPE_INT: sprintf(&stack[s_size * buf_s], "%ld", v.as.uuc_int); break;
                case TYPE_DOUBLE: sprintf(&stack[s_size * buf_s], "%.2f", v.as.uuc_double); break;
                default: break;
            }
            s_size++;
        } else if (code >= OP_ADD && code <= OP_DIVIDE) {
            s_size--;
            char *a = &stack[s_size * buf_s];
            s_size--;
            char *b = &stack[s_size * buf_s];
            sprintf(bb, "( %c %s %s )", opcode_op_char(code), b, a);
            memcpy(&stack[s_size * buf_s], bb, buf_s);
            s_size++;
        } else if (code >= OP_NEGATE && code <= OP_NOT) {
            s_size--;
            char *a = &stack[s_size * buf_s];
            sprintf(bb, "( %c %s )", opcode_op_char(code), a);
            memcpy(&stack[s_size * buf_s], bb, buf_s);
            s_size++;
        }
    }
    sprintf(buf, "%s", stack);
    free(stack);
}

char opcode_op_char(OpCode opcode) {
    switch(opcode) {
        case OP_ADD: return '+';
        case OP_SUBSTRACT: return '-';
        case OP_MULTIPLY: return '*';
        case OP_DIVIDE: return '/';
        case OP_NOT: return '!';
        case OP_NEGATE: return '-';
        default: return 'e';
    }
}
