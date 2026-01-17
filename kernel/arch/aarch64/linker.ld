ENTRY(_start)

PHDRS
{
    text PT_LOAD FLAGS(5);   /* R + X */
    rodata PT_LOAD FLAGS(4); /* R */
    data PT_LOAD FLAGS(6);   /* R + W */
}

SECTIONS
{
    .text : {
        *(.text*)
        *(.text.*)
        _text_start = .;
    } :text

    .rodata : {
        *(.rodata*)
        *(.rodata.*)
        _rodata_start = .;
    } :rodata

    .data : {
        *(.data*)
        *(.data.*)
        _data_start = .;
    } :data

    .bss : {
        *(.bss*)
        *(COMMON)
        _bss_start = .;
        . = ALIGN(0x1000);
    } :data

    PROVIDE(kernel_size = .);
}
