[bits 64]

GLOBAL JumpIntoUserspace
EXTERN TSSSetStack

JumpIntoUserspace:
	; Save parameters
	push	rsi
	push	rdi
	; Quickly save our stack pointer
	mov		rdi, 0
	mov		rsi, rsp
	add		rsi, 16				; compensate for our saved parameters 2 * 0x08
	call	TSSSetStack
	; Enter into userspace
	pop		rcx					; Former rdi parameter, used to locate the code in userspace
	pop		rsp					; Former rsi parameter, userspace stack. Must be last popped (obviously)
	mov		r11, 0x0202			; RFLAGS
	o64 sysret
