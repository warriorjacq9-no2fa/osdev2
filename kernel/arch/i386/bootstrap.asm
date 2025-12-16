[org 0x7C00]
[bits 16]

_start:
    cli
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax

    ;==========================
    ; Set VGA mode
    ;==========================
    mov ax, 0x0003 ; 80x25
    int 0x10

    ;==========================
    ; Load kernel
    ;==========================
    mov bx, 0x0000          ; offset in segment
    mov ax, 0x1000
    mov es, ax
    mov ax, 0x0263          ; Read 63 sectors
    mov ch, 0               ; cylinder 0
    mov cl, 2               ; sector 2 (first sector of kernel)
    mov dh, 0               ; head 0
    ; BIOS sets hard disk number for us
read_loop:
    int 0x13
    jc disk_error
    cli
    ; now kernel is at 0x10000

    ;==========================
    ; Setup GDT
    ;==========================
    lgdt [gdt_descriptor]

    ;==========================
    ; Enter protected mode
    ;==========================
    mov eax, cr0
    or eax, 1               ; set PE bit
    mov cr0, eax

    ; far jump to flush prefetch + load CS
    jmp 0x08:protected_mode_start

disk_error:
    hlt

;==========================
; Protected mode start
;==========================
[bits 32]
protected_mode_start:
    ; Setup data segments
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    ; Jump to kernel entry point at 0x10000
    mov eax, 0x0010000
    jmp eax
    hlt

;==========================
; GDT
;==========================
gdt_start:
    ; Null descriptor
    dq 0x0
    ; Code segment
    dq 0x00CF9B000000FFFF
    ; Data segment
    dq 0x00CF92000000FFFF
gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start

;==========================
; Boot signature
;==========================
times 510-($-$$) db 0
db 0x55
db 0xAA
