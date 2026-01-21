#include "boot.h"

#define UART_RBR(base)  (*(volatile uint8_t *)(base + 0x00)) // read
#define UART_THR(base)  (*(volatile uint8_t *)(base + 0x00)) // write
#define UART_IER(base)  (*(volatile uint8_t *)(base + 0x04))
#define UART_LCR(base)  (*(volatile uint8_t *)(base + 0x0C))
#define UART_LSR(base)  (*(volatile uint8_t *)(base + 0x14))

static uint64_t* uart_base;
static uint16_t uart_model;

void uart_init(serial_info_t serial) {
    uart_base = (uint64_t*) serial.base;
    uart_model = serial.model;
    UART_LCR(uart_base) = 0x80;
    UART_RBR(uart_base) = 1;
    UART_IER(uart_base) = 0;
    UART_LCR(uart_base) = 0x03;
}

void uart_putc(char c) {
    *uart_base = c;
}

int uart_poll(char* c) {
    if((UART_LSR(uart_base) & 0x01)) {
        *c = UART_RBR(uart_base);
        return 1;
    }
    return 0;
}

char uart_getc() {
    while ((UART_LSR(uart_base) & 0x01) == 0);

    return UART_RBR(uart_base);
}