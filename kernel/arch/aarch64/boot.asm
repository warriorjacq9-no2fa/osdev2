/* _start.s - ARM64 PIE kernel entry */
.section .text.start
.global _start

/* On entry:
 * x0 = boot_info pointer
 * x1 = PIE base address (loader must pass base_addr)
 */
_start:
    /* x0 = boot_info */
    /* x1 = base address */

    mov x19, x0
    mov x20, x1

    /* --- Switch to EL1 --- */

    mov x0, #(1 << 31)
    msr hcr_el2, x0

    mov x0, #0x0800
    movk x0, #0x30d0, lsl #16
    msr sctlr_el1, x0

    mov x0, #0x33ff
    msr cptr_el2, x0

    msr hstr_el2, xzr

    mov x0, #0x3c5
    msr spsr_el2, x0

    adr x0, jmp_el1
    msr elr_el2, x0

    adr x0, stack_top
    msr sp_el1, x0

    eret

jmp_el1:
    /* Set up interrupts */
    adr x0, vector_table
    msr vbar_el1, x0
    msr daifclr, #0xF

    /* Call the boot_info parsing code, which is in C */
    mov x0, x19
    bl parse_boot_info

    bl kmain

halt:
    wfe
    b halt

exception_entry:
    msr daifset, #0xF
    sub sp, sp, #192
    stp x0, x1, [sp, #16 * 0]
    stp x2, x3, [sp, #16 * 1]
    stp x4, x5, [sp, #16 * 2]
    stp x6, x7, [sp, #16 * 3]
    stp x8, x9, [sp, #16 * 4]
    stp x10, x11, [sp, #16 * 5]
    stp x12, x13, [sp, #16 * 6]
    stp x14, x15, [sp, #16 * 7]
    stp x16, x17, [sp, #16 * 8]
    stp x18, x29, [sp, #16 * 9]
    stp x30, xzr, [sp, #16 * 10]

    mrs x0, ESR_EL1
    mrs x1, FAR_EL1
    stp x0, x1, [sp, #16 * 11]

    mov x0, sp
    bl exception_handler

    ldp x0, x1, [sp, #16 * 0]
    ldp x2, x3, [sp, #16 * 1]
    ldp x4, x5, [sp, #16 * 2]
    ldp x6, x7, [sp, #16 * 3]
    ldp x8, x9, [sp, #16 * 4]
    ldp x10, x11, [sp, #16 * 5]
    ldp x12, x13, [sp, #16 * 6]
    ldp x14, x15, [sp, #16 * 7]
    ldp x16, x17, [sp, #16 * 8]
    ldp x18, x29, [sp, #16 * 9]
    ldp x30, xzr, [sp, #16 * 10]

    add sp, sp, #192
.wait:
    wfe
    b .wait

interrupt_entry:
    sub sp, sp, #192
    stp x0, x1, [sp, #16 * 0]
    stp x2, x3, [sp, #16 * 1]
    stp x4, x5, [sp, #16 * 2]
    stp x6, x7, [sp, #16 * 3]
    stp x8, x9, [sp, #16 * 4]
    stp x10, x11, [sp, #16 * 5]
    stp x12, x13, [sp, #16 * 6]
    stp x14, x15, [sp, #16 * 7]
    stp x16, x17, [sp, #16 * 8]
    stp x18, x29, [sp, #16 * 9]
    stp x30, xzr, [sp, #16 * 10]

    mrs x0, ESR_EL1
    mrs x1, FAR_EL1
    stp x0, x1, [sp, #16 * 11]

    mov x0, sp
    bl interrupt_handler

    ldp x0, x1, [sp, #16 * 0]
    ldp x2, x3, [sp, #16 * 1]
    ldp x4, x5, [sp, #16 * 2]
    ldp x6, x7, [sp, #16 * 3]
    ldp x8, x9, [sp, #16 * 4]
    ldp x10, x11, [sp, #16 * 5]
    ldp x12, x13, [sp, #16 * 6]
    ldp x14, x15, [sp, #16 * 7]
    ldp x16, x17, [sp, #16 * 8]
    ldp x18, x29, [sp, #16 * 9]
    ldp x30, xzr, [sp, #16 * 10]
    eret

.global vector_table
.balign 2048
vector_table:
    b .
.balign 128
    b .
.balign 128
    b .
.balign 128
    b .
.balign 128
    b exception_entry
.balign 128
    b interrupt_entry
.balign 128
    b interrupt_entry
.balign 128
    b exception_entry

.section .text
.extern parse_boot_info
.extern interrupt_handler
.extern exception_handler
.extern kmain

/* --- Data sections --- */
.section .bss
.align 16
stack:
    .skip 0x4000
stack_top:
