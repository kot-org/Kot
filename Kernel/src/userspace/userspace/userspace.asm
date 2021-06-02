[bits 64]

GLOBAL JumpIntoUserspace
EXTERN TSSSetStack

JumpIntoUserspace:
	cli

	; Save parameters
	
	push	rsi ; user stack
	push	rdi ; user function
	push 	rdx ; gdt user code
	push 	rcx ; gdt user data


	; Quickly save our stack pointer

	mov		rdi, 0
	mov		rsi, rsp
	add		rsi, 32				; compensate for our saved parameters 4 * 0x08
	call	TSSSetStack


	; Enter into userspace

	pop rdx  ; pop gdt user data
	pop rsi  ; pop gdt user code
	pop rax  ; pop user function
	pop rdi  ; pop user stack
	

	mov r11, 0x202 ; interrupts and syscalls 

	push rdx ; push ss
	push rdi ; push stack
	push r11 ; push rflags
	push rsi ; push cs
	push rax ; push rip
	iretq
