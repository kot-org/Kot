[bits 64]

GLOBAL LaunchUserSpace, ExitAndSetBit

LaunchUserSpace:
	sti
	int 0x40 
	jmp LaunchUserSpace

ExitAndSetBit:
	mov byte [rdi], 1
.loop:
	int 0x40
	jmp .loop