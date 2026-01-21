/* _start.s - ARM64 PIE kernel entry */
.section .text
.global _start
.type _start, %function

/* On entry:
 * x0 = boot_info pointer
 * x1 = PIE base address (loader must pass base_addr)
 */
_start:
    /* x0 = boot_info */
    mov x19, x0          /* save boot_info pointer */
    mov x20, x1          /* save PIE base address */

    /* Set up stack: stack_top label is relative to PIE base */
    adr x1, stack_top
    mov sp, x1

    /* UART base (from ACPI/SPCR, can also be hardcoded for now) */
    mov x0, 0x09000000

    /* Print message */
    adr x1, msg

1:  ldrb w2, [x1], #1
    cbz w2, 2f
    str w2, [x0]
    b 1b

2:
halt:
    wfe
    b halt

/* --- Data sections --- */
.section .rodata
msg:
    .ascii "Hello from ARM64 PIE kernel!\n\0"

.section .bss
.align 16
stack:
    .skip 0x4000
stack_top:
