[bits 64]

GLOBAL LaunchUserSpace

LaunchUserSpace:
	sti
	int 0x40 
	ret