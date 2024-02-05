OUTPUT_FORMAT(elf64-x86-64)
OUTPUT_ARCH(i386:x86-64)

ENTRY(kernel_entry)

PHDRS
{
    text   PT_LOAD FLAGS((1 << 0) | (1 << 1) | (1 << 2));
    rodata PT_LOAD FLAGS((1 << 0) | (1 << 1) | (1 << 2));
    data   PT_LOAD FLAGS((1 << 0) | (1 << 1) | (1 << 2));
}

SECTIONS
{
    . = 0xffffffff80000000;
    kernel_start = .;

    .text :
    {
        *(.text*)
    } :text

    . += CONSTANT(MAXPAGESIZE);

    .rodata : 
    {
        *(.rodata*)
    } :rodata

    . += CONSTANT(MAXPAGESIZE);

    .data : 
    {
        *(.data*)
    } :data

    . += CONSTANT(MAXPAGESIZE);

    .bss :
    {
        *(COMMON)
        *(.bss*)
    } :data

    . += CONSTANT(MAXPAGESIZE);

    kernel_end = .;
}
