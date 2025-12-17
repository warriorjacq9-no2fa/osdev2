.section .init
.option norvc
.global _start
.type _start, @function

_start:
    /* Set global pointer */
.option push
.option norelax
    la gp, global_pointer
.option pop

    /* Disable paging (safe in S-mode) */
    csrw satp, zero

    /* Set stack */
    la sp, stack_top

    /* Clear BSS */
    la t0, bss_start
    la t1, bss_end
1:
    bgeu t0, t1, 2f
    sd zero, 0(t0)
    addi t0, t0, 8
    j 1b
2:

    /* Jump to kernel main */
    call kmain

3:  j 3b
