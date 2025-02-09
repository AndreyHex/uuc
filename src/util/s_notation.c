#include "../include/s_notation.h"
#include "../include/bytecode.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

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
            sprintf(&stack[s_size * buf_s], "%.2f", slice->constants.head[index]);
            s_size++;
        } else if (code >= OP_ADD && code <= OP_DIVIDE) {
            s_size--;
            char *a = &stack[s_size * buf_s];
            s_size--;
            char *b = &stack[s_size * buf_s];
            sprintf(bb, "( %s %s %s )", opcode_name(code), b ,a);
            memcpy(&stack[s_size * buf_s], bb, buf_s);
            s_size++;
        }
    }
    sprintf(buf, "%s", stack);
    free(stack);
}
