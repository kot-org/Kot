[bits 64]

GLOBAL LaunchUserSpace, ForceSchedule, ForceSelfDestruction

LaunchUserSpace:
	sti
	jmp ForceSchedule

ForceSchedule:
	int 0x41
	jmp ForceSchedule

ForceSelfDestruction:
	int 0x42
	jmp ForceSelfDestruction