#ifndef VGA_H
#define VGA_H
#include <stdint.h>

#define WIDTH 80
#define HEIGHT 25

#define vga_entry(s, c) (((s & 0xFF) << 8) | (c & 0xFF))

void putc(char c, uint8_t x, uint8_t y);
void puts(char* s);
void vga_init();

#endif