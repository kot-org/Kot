[bits 64]

GLOBAL LaunchUserSpace

LaunchUserSpace:
	swapgs
	sti
	int 0x40 
	ret