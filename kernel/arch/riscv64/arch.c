#include <kernel/arch.h>
#include <stdio.h>

void wait() {
    asm volatile("wfi");
}

void arch_init(void (*kcall)(char, unsigned char)) {
    puts("Hello, world!");
}