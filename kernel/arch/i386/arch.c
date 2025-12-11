#include "vga.h"

void wait() {
    asm volatile("hlt");
}

void arch_init() {
    vga_init();
}