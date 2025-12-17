#include "vga.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "vga.h"

static uint8_t* UART_MEM = (uint8_t*)0x10000000;

void vga_init() {
}

void putc(char c) {
    if(c == 0) return;
    *UART_MEM = c;
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