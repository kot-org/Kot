OUTPUT_ARCH(i386:x86-32)

ENTRY(_start)

PHDRS
{
    null        PT_NULL    FLAGS(0) ;          
    multiboot   PT_LOAD    FLAGS((1 << 0) | (1 << 1) | (1 << 2)) ; 
    text        PT_LOAD    FLAGS((1 << 0) | (1 << 1) | (1 << 2)) ; 
    rodata      PT_LOAD    FLAGS((1 << 0) | (1 << 1) | (1 << 2)) ;         
    data        PT_LOAD    FLAGS((1 << 0) | (1 << 1) | (1 << 2)) ; 
}

SECTIONS
{
    . = 0x00100000;
    .multiboot : {
        *(.multiboot .multiboot.*)
    } :multiboot
    
    . += CONSTANT(MAXPAGESIZE);
    
    .text : {
        *(.text .text.*)
    } :text

    . += CONSTANT(MAXPAGESIZE);

    .rodata : {
        *(.rodata .rodata.*)
    } :rodata

    . += CONSTANT(MAXPAGESIZE);

    .data : {
        *(.data .data.*)
    } :data

    .bss : {
        *(COMMON)
        *(.bss .bss.*)
    } :data
}