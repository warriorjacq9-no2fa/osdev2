#include <stddef.h>
#include <kernel/arch.h>
#include <stdio.h>

// Keyboard callback
void kcallback(char c, unsigned char kstate) {
    putc(c);
}

int kmain(size_t* mbi) {
    arch_init(kcallback);

    printf("Hello World!\n");

    for(;;) wait();
}