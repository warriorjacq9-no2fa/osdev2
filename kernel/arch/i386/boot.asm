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

%macro isr_err 1
isr_stub_%+%1:
    push dword %1
    call exception_handler_err
    add esp, 4
    hlt
%endmacro

%macro isr_noerr 1
isr_stub_%+%1:
    push dword %1
    call exception_handler
    add esp, 4
    hlt
%endmacro

%macro isr_res 1
isr_stub_%+%1:
    hlt
%endmacro

extern exception_handler
extern exception_handler_err

isr_noerr   0
isr_noerr   1
isr_noerr   2
isr_noerr   3
isr_noerr   4
isr_noerr   5
isr_noerr   6
isr_noerr   7
isr_err     8
isr_noerr   9
isr_err     10
isr_err     11
isr_err     12
isr_err     13
isr_err     14
isr_res     15
isr_noerr   16
isr_err     17
isr_noerr   18
isr_noerr   19
isr_noerr   20
isr_err     21
isr_res     22
isr_res     23
isr_res     24
isr_res     25
isr_res     26
isr_res     27
isr_noerr   28
isr_err     29
isr_err     30
isr_res     31

global gdt_r

section .rodata

gdt:
; Null descriptor
dq 0x0000000000000000
; Kernel code descriptor
dq 0x00CF9B000000FFFF
; Kernel data descriptor
dq 0x00CF92000000FFFF
; TSS descriptor
dq 0x0000890000000000

gdt_r:
; Limit (32 bytes)
dw 0x1F
; Base
dd gdt

global isr_stub_table
isr_stub_table:
%assign i 0
%rep 32
    dd isr_stub_%+i
%assign i i+1
%endrep
