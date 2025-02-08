#include <stdint.h>
#include <stdio.h>
#include "include/vm.h"
#include "include/parser.h"

int main(int argc, const char *argv[]) {
    printf("hello wordld\n");


    Slice slice = parse_code(" 3 + 4");
    slice_print(&slice);
    VM vm = vm_init(slice);
    vm_run(&vm);

    return 0;
}
