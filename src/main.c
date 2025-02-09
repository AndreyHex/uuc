#include <stdint.h>
#include <stdio.h>
#include "include/vm.h"
#include "include/parser.h"

int main(int argc, const char *argv[]) {
    printf("hello wordld\n");


    Slice slice = parse_code(" 1 + 2 * 3 / 4 + 5 - 6 + 7 / 8 * 9;");
    slice_print(&slice);
    VM vm = vm_init(slice);
    vm_run(&vm);

    return 0;
}
