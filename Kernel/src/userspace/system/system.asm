[bits 64]

%define SYSCALL_EXIT	60

%macro	SYSCALL_SAVE	0
	push	r15
	push	r14
	push	r13
	push	r12
	push	r10
	push	r9
	push	r8
	push	rbp
	push	rdi
	push	rsi
	push	rdx
	push	rbx	
%endmacro
%macro	SYSCALL_RESTORE	0
	pop	rbx
	pop	rdx
	pop	rsi
	pop	rdi
	pop	rbp
	pop	r8
	pop	r9
	pop	r10
	pop	r12
	pop	r13
	pop	r14
	pop	r15
%endmacro


GLOBAL EnableSystemCall
EXTERN TSSGetStack, SyscallEntry, SystemExit


EnableSystemCall:
	; Load handler RIP into LSTAR MSR
	mov		rax, syscall_entry
	mov		rdx, rax
	shr		rdx, 0x20
	mov		rcx, 0xc0000082
	wrmsr
	; Enable syscall / sysret instruction
	mov		rcx, 0xc0000080
	rdmsr
	or		eax, 1
	wrmsr
	; Load segments into STAR MSR
	mov		rcx, 0xc0000081
	rdmsr
	mov		edx, 0x00180008
	wrmsr
	ret      

syscall_entry:
	cli

	; Save and switch context
	SYSCALL_SAVE
	push	r11
	push	rcx
 
	; Get syscall handler and call the routine
	mov		r14, rax
	mov 	r15, r10

	call	SyscallEntry

	mov		rax, rbx		; save the function pointer return of syscall
	mov		rcx, r10		; syscall's 4th param and sys v abi's 4th param are the only misaligned parameters
	call	rbx				; call the returned function pointer

	; Restore state-sensitive information and exit
	pop		rcx
	pop		r11
	lea		rdx, [rel SystemExit]
	cmp		rbx, rdx
	je		.kernel_exit
	.sysret_exit:
		SYSCALL_RESTORE		
		o64	sysret
		sti
		ret
	.kernel_exit:
		mov		rdi, 0
		call	TSSGetStack
		SYSCALL_RESTORE
		mov		rsp, rax
		sti
		ret

