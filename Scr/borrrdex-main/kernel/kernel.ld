OUTPUT_FORMAT(elf64-x86-64)
ENTRY(_start)
phys = 0x100000;

SECTIONS
{
    .text phys : AT(phys)
    {
        _KernelStart = .;
        *(.boot*)
        *(.text*)
        . = ALIGN(4096);
    }

    _ModulesStart = .;
    modules_ptr : {
        *(modules_ptr)
    }
    _ModulesEnd = .;

    real_modules_ptr : {
        *(real_modules_ptr)
    }

    .data : AT(phys + (data - _KernelStart))
    {
        data = .;
        *(.data)
        PROVIDE(kernel_symbols_start = .);
        PROVIDE(kernel_symbols_end = .);
        . = ALIGN(4096);
    }
    .rodata : AT(phys + (rodata - _KernelStart))
    {
        rodata = .;
        *(.rodata)
        . = ALIGN(4096);
    }
    .bss : AT(phys + (bss - _KernelStart))
    {
        bss = .;
        *(.bss)
        . = ALIGN(4096);
    }
    _KernelEnd = .;

    /DISCARD/ :
    {
    *(.comment)
    *(.note.gnu.build-id)
    }
}