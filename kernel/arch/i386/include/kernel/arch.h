#ifndef ARCH_H
#define ARCH_H

#include <stdint.h>

#define be32(i) (((i >> 24) & 0xFF) | ((i >> 8) & 0xFF00) | \
    ((i << 8) & 0xFF0000) | ((i << 24) & 0xFF000000))
#define be16(i) (((i >> 8) & 0xFF) | ((i << 8) & 0xFF00))

void wait();

void arch_init();
void puts(char* s);
void putc(char c);

#endif