OUTPUT_FORMAT(elf64-littleaarch64)

ENTRY(kernel_entry)

PHDRS
{
    text   PT_LOAD FLAGS((1 << 0) | (1 << 2));
    rodata PT_LOAD FLAGS((1 << 2));
    data   PT_LOAD FLAGS((1 << 1) | (1 << 2));
}

SECTIONS
{
    . = 0xffffffff80000000;

    .text :
    {
        TEXT_BEGIN = .;
        *(.text*)
        TEXT_END = .;
    } :text

    . += CONSTANT(MAXPAGESIZE);

    .rodata : 
    {
        RODATA_BEGIN = .;
        *(.rodata*)
        RODATA_END = .;
    } :rodata

    . += CONSTANT(MAXPAGESIZE);

    .data : 
    {
        DATA_BEGIN = .;
        *(.data*)
    } :data

    . += CONSTANT(MAXPAGESIZE);

    .bss :
    {
        *(COMMON)
        *(.bss*)
        DATA_END = .;
    } :data

    . += CONSTANT(MAXPAGESIZE);

    KERNEL_SIZE = . - 0xffffffff80000000;

}
