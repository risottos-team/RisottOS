OUTPUT_FORMAT(binary)
ENTRY(_start)

SECTIONS
{
    . = 0x500000;

    .text : ALIGN(1)
    {
        *(.text.start)
        *(.text)
    }

    .rodata : ALIGN(4)
    {
        *(.rodata*)
    }

    .data : ALIGN(4)
    {
        *(.data)
    }

    .bss : ALIGN(4)
    {
        *(.bss)
        *(COMMON)
    }
}
