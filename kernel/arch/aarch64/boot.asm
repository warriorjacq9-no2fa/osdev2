.section .text

_start:
    /* On entry:
     * x0 = Boot info pointer
     * x1-x3 = 0
     */
    ldr x1, =stack_top
    mov sp, x1

    /* Optional: save boot info pointer */
    mov x19, x0

    /* UART base */
    mov x0, 0x09000000
    ldr x1, =msg

1:  ldrb w2, [x1], #1
    cbz w2, 2f
    str w2, [x0]
    b 1b

2:
halt:
    wfe
    b halt

.section .rodata
msg:
    .ascii "Hello from ARM64 kernel!\n\0"

.section .bss
.align 16
stack:
    .skip 0x4000
stack_top:
