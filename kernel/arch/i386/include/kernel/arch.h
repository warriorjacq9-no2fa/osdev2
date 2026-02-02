#ifndef ARCH_H
#define ARCH_H

#include <stdint.h>

#define be32(i) (((i >> 24) & 0xFF) | ((i >> 8) & 0xFF00) | \
    ((i << 8) & 0xFF0000) | ((i << 24) & 0xFF000000))
#define be16(i) (((i >> 8) & 0xFF) | ((i << 8) & 0xFF00))

void wait();

void arch_init(void (*kcall)(char, unsigned char));
void puts(char* s);
void putc(char c);
void setpos(uint8_t x, uint8_t y);
void setpos_x(uint8_t x);
uint16_t getpos();
void clrline_y(uint8_t y);
void clrline();

#endif