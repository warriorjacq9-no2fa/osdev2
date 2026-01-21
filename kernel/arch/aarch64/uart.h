#ifndef UART_H
#define UART_H

#include "boot.h"

void uart_init(serial_info_t serial);
int uart_poll(char* c);

#endif