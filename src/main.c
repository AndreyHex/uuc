#include <stdint.h>
#include <stdio.h>
#include "include/uuc_collection.h"
#include "include/uuc_type.h"
#include "include/uuc_vm.h"
#include "include/uuc_parser.h"

int main(int argc, const char *argv[]) {
    printf("hello\n");
    Slice slice;
    UucResult r = parse_code(&slice, "var a = 1; var b = 2; var c = a + b;");
    VM vm = uuc_vm_init(slice);
    UucResult vmr = uuc_vm_run(&vm);
    uuc_vm_dump(&vm);
    return 0;
}
