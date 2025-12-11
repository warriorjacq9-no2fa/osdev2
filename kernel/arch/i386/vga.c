#include <stdint.h>
#include <stdio.h>
#include "vga.h"

static uint16_t* VGA_MEM = (uint16_t*)0xB8000;

static uint8_t tx, ty;

void vga_init() {
    tx = 0;
    ty = 0;
}

void cursor(uint8_t x, uint8_t y) {
    uint16_t pos = y * WIDTH + x;

    outb(0x3D4, 0x0F);
    outb(0x3D5, (uint8_t)(pos & 0xFF));
    outb(0x3D4, 0x0E);
    outb(0x3D5, (uint8_t)((pos >> 8) & 0xFF));
}

void putc(char c, uint8_t x, uint8_t y) {
    cursor(x + 1, y);
    if(c == 0) return;
    uint16_t off = y * WIDTH + x;
    VGA_MEM[off] = vga_entry(0x07, c);
}

void puts(char* s) {
    if(s == 0) return;
    int i;
    char c;
    while((c = *s++) != 0){
        if(tx >= WIDTH) {
            tx = 0;
            ty += 1;
        }
        if(c == '\n' || c == '\t') {
            tx = 0;
            ty += 1;
            c = 0;
            cursor(tx, ty);
        } else
            putc(c, tx++, ty);
    }
}