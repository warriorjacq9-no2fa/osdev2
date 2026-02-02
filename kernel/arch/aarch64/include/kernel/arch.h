#ifndef ARCH_H
#define ARCH_H

#include <stdint.h>

#define be32(i) (((i >> 24) & 0xFF) | ((i >> 8) & 0xFF00) | \
    ((i << 8) & 0xFF0000) | ((i << 24) & 0xFF000000))
#define be16(i) (((i >> 8) & 0xFF) | ((i << 8) & 0xFF00))

typedef struct {
    uint64_t x0, x1, x2, x3, x4, x5, x6, x7;
    uint64_t x8, x9, x10, x11, x12, x13, x14;
    uint64_t x15, x16, x17, x18, fp, elr, xzr;
    uint64_t esr, far;
} interrupt_frame_t;

void wait();

void arch_init(void (*kcall)(char, unsigned char));
void puts(char* s);
void putc(char c);
void setpos(uint8_t x, uint8_t y);
void setpos_x(uint8_t x);
uint16_t getpos();
void clrline();

#endif