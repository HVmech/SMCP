ENTRY(Reset_Handler)

MEMORY
{
    FLASH (rx) : ORIGIN = 0x08000000, LENGTH = 64K
    RAM (xrw)  : ORIGIN = 0x20000000, LENGTH = 20K
}

_stack = ORIGIN(RAM) + LENGTH(RAM);
_estack = _stack;

_stext = ORIGIN(FLASH);
_etext = _stext + SIZEOF(.text);

_sdata = ADDR(.data);
_edata = ADDR(.data) + SIZEOF(.data);

_sbss = ADDR(.bss); 
_ebss = ADDR(.bss) + SIZEOF(.bss);

_end = _ebss;
PROVIDE(end = _end);

SECTIONS
{
    .isr_vector :
    {
        . = ALIGN(4);
        KEEP(*(.isr_vector))
        . = ALIGN(4);
    } >FLASH

    .text :
    {
        . = ALIGN(4);
        *(.text)
        *(.text*)
        *(.rodata)
        *(.rodata*)
        . = ALIGN(4);
    } >FLASH

    .data :
    {
        . = ALIGN(4);
        *(.data)
        *(.data*)
        . = ALIGN(4);
    } >RAM AT>FLASH

    .bss :
    {
        . = ALIGN(4);
        *(.bss)
        *(.bss*)
        *(COMMON)
        . = ALIGN(4);
    } >RAM
}