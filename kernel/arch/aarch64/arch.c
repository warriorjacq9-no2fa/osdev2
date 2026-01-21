#include <kernel/arch.h>
#include "boot.h"
#include "uart.h"

void (*kcallback)(char, unsigned char);

void parse_boot_info(boot_info_t* boot_info) {
    uart_init(boot_info->serial);
}

void wait(){
    asm volatile("wfe");
}

void arch_init(void (*kcall)(char, unsigned char)){
    kcallback = kcall;
}

void putc(char c){
}

void puts(char* s){
    if(s == 0) return;
    char c;
    while((c = *s++)) {
        putc(c);
    }
}
void setpos(uint8_t x, uint8_t y){

}
uint16_t getpos(){
    return 0;
}
void clrline(uint8_t y){
    
}