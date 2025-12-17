#include "vga.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "vga.h"

void vga_init() {
}

void putc(char c) {
    if(c == 0) return;
    register long a0 asm("a0") = c;
    register long a7 asm("a7") = 1; // SBI console_putchar
    asm volatile ("ecall" : : "r"(a0), "r"(a7));
}

void puts(char* s) {
    if(s == 0) return;
    char c;
    while((c = *s++) != 0){
        putc(c);
    }
}

void setpos(uint8_t x, uint8_t y) {
}

uint16_t getpos() {
}

void clrline(uint8_t y) {
}