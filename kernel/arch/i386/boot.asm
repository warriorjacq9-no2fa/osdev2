section .bss
stack_bottom:
resb 4096 ; 4KB of stack
stack_top:

section .text.start
boot_start:
; Get into C as fast as possible
    mov esp, stack_top
    call kmain

halt:
    hlt
    jmp halt

section .start
extern kmain

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

global keyboard_stub
extern keyboard
keyboard_stub:
    pusha
    call keyboard
    popa
    iret

section .rodata

global isr_stub_table
isr_stub_table:
%assign i 0
%rep 32
    dd isr_stub_%+i
%assign i i+1
%endrep
