[bits 64]

GLOBAL LaunchUserSpace, ForceSchedule, ForceSelfDestruction, ForceSelfPause

LaunchUserSpace:
	sti
	jmp ForceSchedule

ForceSchedule:
	int 0x41
	jmp ForceSchedule

ForceSelfDestruction:
	int 0x42
	jmp ForceSelfDestruction

ForceSelfPause:
	int 0x43
	ret