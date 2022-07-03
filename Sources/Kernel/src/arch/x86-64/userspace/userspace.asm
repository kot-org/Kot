[bits 64]

GLOBAL LaunchUserSpace, ForceSchedule, ForceScheduleAndSetBit, ForceScheduleAndClearBit

LaunchUserSpace:
	sti
	int 0x40 
	jmp LaunchUserSpace

ForceSchedule:
	sti
	int 0x40
	jmp ForceSchedule

ForceScheduleAndSetBit:
	mov byte [rdi], 1
	jmp ForceSchedule

ForceScheduleAndClearBit:
	mov byte [rdi], 0
	jmp ForceSchedule