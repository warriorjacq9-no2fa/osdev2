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
    /* x1 = base address */

    /* Set up stack: stack_top label is relative to PIE base */
    adr x1, stack_top
    mov sp, x1

    /* Call the boot_info parsing code, which is in C */
    bl parse_boot_info
    bl kmain

halt:
    wfe
    b halt

.extern parse_boot_info
.extern kmain

/* --- Data sections --- */
.section .bss
.align 16
stack:
    .skip 0x4000
stack_top:
