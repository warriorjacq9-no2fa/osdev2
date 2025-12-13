#include <kernel/arch.h>
#include <stdio.h>
#include "vga.h"

typedef struct {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed)) tabreg_t;

typedef struct {
    uint16_t    offset_l;
    uint16_t    segment;
    uint8_t     reserved;
    uint8_t     flags;
    uint16_t    offset_h;
} __attribute__((packed)) idtent_t;

extern tabreg_t gdt_r;
extern uint32_t isr_stub_table[32];

static idtent_t idt[256];
tabreg_t idt_r;

void wait() {
    asm volatile("hlt");
}

void add_idt_entry(int v, uint32_t isr, uint8_t flags) {
    idt[v].offset_l = isr & 0xFFFF;
    idt[v].offset_h = (isr >> 16) & 0xFFFF;
    idt[v].flags = flags;
    idt[v].reserved = 0;
    // Index 1 (3-15), use GDT (2), Ring 0 (0-1)
    idt[v].segment = 0x0008;
}

void arch_init() {
    // Load GDT
    asm volatile("lgdt %0" : : "m" (gdt_r) : "memory");

    // Fill out IDT and load it
    for(int i = 0; i < 32; i++) {
        // Flags: Present (7), Ring 0 (5-6), Trap gate (0-3)
        add_idt_entry(i, isr_stub_table[i], 0x8E);
    }
    idt_r.base = (uint32_t) &idt[0];
    idt_r.limit = sizeof(idtent_t) * 32 - 1;
    asm volatile("lidt %0" : : "m" (idt_r) : "memory");

    // Before enabling interrupts, we need to disable the PIC
    outb(0x21, 0xFF);
    outb(0xA1, 0xFF);

    asm volatile("sti");
    vga_init();
}

void exception_handler(uint32_t vec) {
    puts("Exception!\n");
}

void exception_handler_err(uint32_t code, uint32_t vec) {
    puts("Exception!\n");
}
