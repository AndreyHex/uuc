#include <stdint.h>
#include <stdio.h>
#include "include/collection.h"
#include "include/uuc_type.h"
#include "include/vm.h"
#include "include/parser.h"

int main(int argc, const char *argv[]) {
    Slice slice = parse_code("\"a\"+\"b\"+\"c\";");
    slice_print(&slice);
    VM vm = vm_init(slice);
    ExeResult s = vm_run(&vm);
    printf("Vm exe status: %d\n", s);
    printf("Result: ");
    type_print(slice.constants.head[0]);
    printf("\n");
    return 0;
}
