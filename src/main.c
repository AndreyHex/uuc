#include <stdint.h>
#include <stdio.h>
#include "include/vm.h"
#include "include/parser.h"
#include "include/s_notation.h"

int main(int argc, const char *argv[]) {
    printf("hello wordld\n");
    char buf[400];

    Slice slice = parse_code("1 + (2 + 3) * 4 + 5;");
    slice_s_notation(&slice, buf, 400);
    printf("%s\n", buf);

    slice_print(&slice);
    VM vm = vm_init(slice);
    vm_run(&vm);

    printf("Result: %f\n", slice.constants.head[0]);

    return 0;
}
