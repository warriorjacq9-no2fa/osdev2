; We are using Multiboot 2 for this kernel
section .multiboot
MAGIC   equ 0xE85250D6
ARCH    equ 0           ; i386 32-bit protected mode
HLEN    equ header_end - $$
CHECK   equ -(MAGIC + ARCH + HLEN)

dd MAGIC
dd ARCH
dd HLEN
dd CHECK
; Multiboot 2 tags
; End tag
dw 0
dw 0
dd 8
header_end:

section .bss
stack_bottom:
resb 4096 ; 4KB of stack
stack_top:

section .text

extern kmain

global _start
_start:
; Get into C as fast as possible
    mov esp, stack_top
    push eax
    call kmain

halt:
    hlt
    jmp halt