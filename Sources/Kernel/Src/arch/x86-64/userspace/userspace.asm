[bits 64]

GLOBAL LaunchUserSpace, ForceSchedule, ForceSelfDestruction, ForceScheduleAndSetBit, ForceScheduleAndClearBit

LaunchUserSpace:
	sti
	jmp ForceSchedule

ForceSchedule:
	int 0x40
	jmp ForceSchedule

ForceSelfDestruction:
	int 0x41
	jmp ForceSelfDestruction

ForceScheduleAndSetBit:
	mov byte [rdi], 1
	jmp ForceSchedule

ForceScheduleAndClearBit:
	mov byte [rdi], 0
	jmp ForceSchedule