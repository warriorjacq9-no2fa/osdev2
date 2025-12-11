#ifndef ARCH_H
#define ARCH_H

#include <stdint.h>

void arch_init();
void puts(char* s);
void putc(char c, uint8_t x, uint8_t y);

#endif