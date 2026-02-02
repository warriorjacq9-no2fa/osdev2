#include <stdint.h>
#include <stdio.h>
#include <string.h>
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

void vga_scroll(void) {
    if (ty >= HEIGHT) {
        memmove(
            &VGA_MEM[0],
            &VGA_MEM[WIDTH],
            (HEIGHT - 1) * WIDTH * sizeof(uint16_t)
        );

        memset(
            &VGA_MEM[(HEIGHT - 1) * WIDTH],
            0,
            WIDTH * sizeof(uint16_t)
        );

        ty = HEIGHT - 1;
    }
}

void putc(char c) {
    if(c == 0) return;
    if(tx >= WIDTH) {
        tx = 0;
        ty += 1;
        vga_scroll();
    }
    if(c == '\n' || c == '\r') {
        tx = 0;
        ty += 1;
        vga_scroll();
        cursor(tx, ty);
    } else {
        cursor(tx + 1, ty);
        uint16_t off = ty * WIDTH + tx;
        VGA_MEM[off] = vga_entry(0x07, c);
        tx++;
    }
}

void puts(char* s) {
    if(s == 0) return;
    char c;
    while((c = *s++) != 0){
        putc(c);
    }
}

void setpos(uint8_t x, uint8_t y) {
    if(x > WIDTH) return;
    tx = x;
    if(y >= HEIGHT) ty = HEIGHT;
    else ty = y;
    vga_scroll();
}

void setpos_x(uint8_t x) {
    if(x > WIDTH) return;
    tx = x;
}

uint16_t getpos() {
    return (ty << 8) | tx;
}

void clrline_y(uint8_t y) {
    memset(VGA_MEM + (y * WIDTH), 0, WIDTH);
}

void clrline() {
    memset(VGA_MEM + (ty * WIDTH), 0, WIDTH);
}