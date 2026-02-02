#include <kernel/arch.h>
#include <stdio.h>
#include "boot.h"
#include "uart.h"

static void (*kcallback)(char, unsigned char);

void parse_boot_info(boot_info_t* boot_info) {
    uart_init(boot_info->serial);
}

void wait(){
    asm volatile("wfe");
}

void arch_init(void (*kcall)(char, unsigned char)){
    kcallback = kcall;
    puts("Press any key...");
    putc(uart_getc());
}

void exception_handler(interrupt_frame_t *frame) {
    printf("Exception ocurred, esr 0x%016X, elr 0x%016X, far 0x%016X\n", frame->esr, frame->elr, frame->far);
}

void interrupt_handler(interrupt_frame_t *frame) {
    uint64_t irq_id;

    asm volatile("mrs %0, ICC_IAR1_EL1" : "=r" (irq_id) :: );

    // Is it the UART?
    uart_int(kcallback);
}

void putc(char c){
    uart_putc(c);
}

void puts(char* s){
    if(s == 0) return;
    char c;
    while((c = *s++)) {
        uart_putc(c);
    }
}

void setpos_x(uint8_t x) {
    
}
void setpos(uint8_t x, uint8_t y){

}
uint16_t getpos(){
    return 0;
}
void clrline(){
}