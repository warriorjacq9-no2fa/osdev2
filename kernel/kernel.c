#include <stddef.h>
#include <kernel/arch.h>
#include <stdio.h>

int kmain(size_t* mbi) {
    arch_init();

    printf("Hello World!\n");

    for(;;) wait();
}