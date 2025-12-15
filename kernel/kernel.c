#include <stddef.h>
#include <kernel/arch.h>
#include <stdio.h>
#include "shell.h"

int kmain() {
    arch_init(kcallback);
    printf("Hello World!\n");
    shell_init();

    for(;;) wait();
}