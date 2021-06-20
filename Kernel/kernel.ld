OUTPUT_FORMAT(elf64-x86-64)
ENTRY(main)

SECTIONS
{
	. = 0x100000;
	_KernelStart = .;


	.text : ALIGN(0x1000)
	{
		IdleTaskStart = .;
		*(.IdleTask)
		IdleTaskEnd = .;
		*(.text)
	}
	.data : ALIGN(0x1000)
	{
		*(.data)
	}
	.rodata : ALIGN(0x1000)
	{
		*(.rodata)
	}
	.bss : ALIGN(0x1000)
	{
		*(COMMON)
		*(.bss)
	}
	_KernelEnd = .;
}