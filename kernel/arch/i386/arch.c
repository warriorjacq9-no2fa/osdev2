#include <kernel/arch.h>
#include <stdio.h>
#include <string.h>
#include "vga.h"

#define KBD_SHIFT   0x01
#define KBD_CAPS    0x02

unsigned char kstate = 0;

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

typedef struct {
   uint32_t eip, cs, eflags;
} registers_t;

extern tabreg_t gdt_r;
extern uint32_t isr_stub_table[32];
extern uint32_t keyboard_stub;
extern void gdt_load();

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

void pic_remap(uint8_t off1, uint8_t off2) {
    outb(0x20, 0x11); // ICW1: indicate presence of ICW4
    iowait();
    outb(0xA0, 0x11);
    iowait();
    outb(0x21, off1); // ICW2: set interrupt vector offsets
    iowait();
    outb(0xA1, off2);
    iowait();
    outb(0x21, 0x2); // ICW3: set cascade identity
    iowait();
    outb(0xA1, 0x2);
    iowait();

    outb(0x21, 0x01); // ICW4: set optional features (8086 mode)
	iowait();
	outb(0x21, 0x01);
	iowait();
}

const char keymap[] = {
    '\0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\0',
    '\0', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    '\0', 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
    '\0', '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', '\0',
    '*', '\0', ' '
};
const char keymap_shift[] = {
    '\0', '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\0',
    '\0', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',
    '\0', 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~',
    '\0', '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', '\0',
    '*', '\0', ' '
};
void keyboard() {
    unsigned char sc = inb(0x60);

    switch (sc) {
        /* Shift pressed */
        case 0x2A: // Left Shift
        case 0x36: // Right Shift
            kstate |= KBD_SHIFT;
            break;

        /* Shift released */
        case 0xAA:
        case 0xB6:
            kstate &= ~KBD_SHIFT;
            break;

        /* Caps Lock pressed */
        case 0x3A:
            kstate ^= KBD_CAPS; // toggle
            break;

        default:
            if (!(sc & 0x80)) {
                char ch;

                if (kstate & KBD_SHIFT)
                    ch = keymap_shift[sc - 1];
                else
                    ch = keymap[sc - 1];

                if (ISALPHA(ch) && (kstate & KBD_CAPS)) {
                    if (ISLOWER(ch))
                        ch -= 32;
                    else
                        ch += 32;
                }

                if (ch)
                    putc(ch);
            }
            break;
    }

    outb(0x20, 0x20); // PIC EOI
}

void arch_init() {
    vga_init();
    // Load GDT
    gdt_load();
    printf("[i386] GDT is at 0x%08X, %u bytes long\n", gdt_r.base, gdt_r.limit);

    // Fill out IDT and load it
    for(int i = 0; i < 32; i++) {
        // Flags: Present (7), Ring 0 (5-6), Exception gate (0-3)
        add_idt_entry(i, isr_stub_table[i], 0x8E);
    }
    idt_r.base = (uint32_t) &idt[0];
    idt_r.limit = sizeof(idtent_t) * 256 - 1;
    asm volatile("lidt %0" : : "m" (idt_r) : "memory");
    printf("[i386] IDT is at 0x%08X, %u bytes long\n", idt_r.base, idt_r.limit);

    // Before enabling interrupts, we need to remap and mask the PIC
    pic_remap(0x20, 0x28);
    
    outb(0x21, 0xFD); // Enable IRQ 1 (Keyboard)
    outb(0xA1, 0xFF);
    add_idt_entry(0x21, (uint32_t)&keyboard_stub, 0x8E);

    asm volatile("sti");
}

void exception_handler(uint32_t vec, registers_t r) {
    printf("Exception 0x%02X at %08X!\n", vec, r.eip);
}

void exception_handler_err(uint32_t vec, uint32_t code, registers_t r) {
    printf("Exception 0x%02X at %08X, error code 0x%X\n", vec, r.eip, code);
}
