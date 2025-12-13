#include <stdio.h>
#include <stdint.h>

void outb(uint16_t port, uint8_t data) {
    asm volatile("outb %b0, %w1" :: "a" (data), "Nd" (port) : "memory");
}

uint8_t inb(uint16_t port) {
    uint8_t data;
    asm volatile("inb %w1, %b0" : "=a" (data) : "Nd" (port) : "memory");
    return data;
}