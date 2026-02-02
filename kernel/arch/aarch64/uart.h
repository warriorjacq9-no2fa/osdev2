#ifndef UART_H
#define UART_H

#include "boot.h"

// NS16550
typedef struct {
    union {
        uint32_t rx;
        uint32_t tx;
        uint32_t dll;
    } u_rthr; // 0x000
    union {
        uint32_t ier;
        uint32_t dlm;
    } u_ier; // 0x004
    uint32_t isr; // 0x008
    uint32_t fcr; // 0x00C
    uint32_t lcr; // 0x010
    uint32_t mcr; // 0x014
    union {
        uint32_t lsr;
        uint32_t psd;
    } u_lsr; // 0x018
    uint32_t msr; // 0x01C
    uint32_t spr; // 0x020
} ns16550_t;

// PL011
typedef struct {
    uint32_t dr; // 0x000
    uint32_t rsr; // 0x004
    uint32_t _res0[4]; // 0x008
    uint32_t fr; // 0x018
    uint32_t _res1; // 0x01C
    uint32_t ilpr; // 0x020
    uint32_t ibrd; // 0x024
    uint32_t fbrd; // 0x028
    uint32_t lcr; // 0x02C
    uint32_t cr; // 0x030
    uint32_t ifls; // 0x034
    uint32_t imsc; // 0x038
    uint32_t ris; // 0x03C
    uint32_t mis; // 0x040
    uint32_t icr; // 0x044
    uint32_t dmacr; // 0x048
} pl011_t;

void uart_init(serial_info_t serial);
int uart_poll(char* c);
char uart_getc();
void uart_putc(char c);
void uart_int(void (*kcallback)(char, unsigned char));

#endif