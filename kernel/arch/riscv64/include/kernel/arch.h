#ifndef ARCH_H
#define ARCH_H
#include <stdint.h>

void wait();
void arch_init(void (*kcall)(char, unsigned char));

void putc(char c);
void puts(char* s);
void setpos(uint8_t x, uint8_t y);
uint16_t getpos();
void clrline(uint8_t y);

#endif