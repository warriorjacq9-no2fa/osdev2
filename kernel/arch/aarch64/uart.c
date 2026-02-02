#include "uart.h"
#include <kernel/arch.h>
#include "boot.h"

static ns16550_t* uart_ns = 0;
static pl011_t* uart_pl = 0;
static uint64_t* uart_fallback = 0;

void uart_init(serial_info_t serial) {
    uart_fallback = (uint64_t*) serial.base;
    switch(serial.model) {
        case 0x00:
        case 0x01:
        case 0x05:
            uart_ns = (ns16550_t*) serial.base;
            uart_ns->lcr = 0x03; // Unset DLAB, 8N1
            puts("[AArch64] UART type is NS16550\n");
            break;
        
        case 0x03:
            uart_pl = (pl011_t*) serial.base;
            uart_pl->cr = 0x00; // Disable UART for config
            uart_pl->lcr = 0x70; // Enable FIFO, 8N1
            uart_pl->imsc |= (1 << 4); // Enable RX interrupt
            uart_pl->cr = 0x0301; // Enable UART, TX, RX
            puts("[AArch64] UART type is PL011\n");
            break;
    }

}

void uart_int(void (*kcallback)(char, unsigned char)) {
    if(uart_pl) {
        if (uart_pl->mis & (1 << 4)) {
            kcallback(uart_getc(), 0);
            uart_pl->icr = (1 << 4);
        }
    }
}

void uart_putc(char c) {
    if(uart_ns) {
        while(!(uart_ns->u_lsr.lsr & 0x20)); // Wait until THR empty
        uart_ns->u_rthr.tx = c;
    } else if(uart_pl) {
        while(uart_pl->fr & 0x20); // Wait on FIFO full
        uart_pl->dr = c;
    } else {
        *uart_fallback = c;
    }
}

int uart_poll(char* c) {
    if(uart_ns) {
        if(uart_ns->u_lsr.lsr & 0x01) {
            *c = uart_ns->u_rthr.rx;
            return 1;
        }
    } else if(uart_pl) {
        if(!(uart_pl->fr & 0x10)) {
            *c = uart_pl->dr;
            return 1;
        }
    }
    return 0;
}

char uart_getc() {
    if(uart_ns) {
        while(!(uart_ns->u_lsr.lsr & 0x01)); // Wait until data ready

        return uart_ns->u_rthr.rx;
    } else if(uart_pl) {
        while(uart_pl->fr & 0x10); // Wait until FIFO not empty

        return uart_pl->dr;
    }
    return 0;
}