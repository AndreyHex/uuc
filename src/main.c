#include <stdint.h>
#include <stdio.h>
#include "include/uuc_type.h"
#include "include/vm.h"
#include "include/parser.h"
#include "include/s_notation.h"

int main(int argc, const char *argv[]) {
    long a = 1;
    double b = 1.0;
    printf("comparing: %d\n", a == b);
    if(1) return 0;
    printf("hello wordld\n");
    char buf[400];

    Slice slice = parse_code("1 + (true + 3) * 4 + 5;");
    slice_s_notation(&slice, buf, 400);
    printf("%s\n", buf);

    slice_print(&slice);
    VM vm = vm_init(slice);
    ExeResult s = vm_run(&vm);

    printf("Vm exe status: %d\n", s);
    printf("Result: ");
    type_print(slice.constants.head[0]);
    printf("\n");

    return 0;
}
