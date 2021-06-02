[bits 64]
ALIGN	4096

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
EXTERN TSSGetStack, TSSSetStack, SyscallEntry, SystemExit


EnableSystemCall:
	; Load handler RIP into LSTAR MSR
	mov		rcx, 0xc0000082
	mov		rax, syscall_entry
	mov		rdx, rax
	shr		rdx, 32
	wrmsr
	; Enable syscall / sysret instruction
	mov		rcx, 0xc0000080
	rdmsr
	or		rax, 1
	wrmsr
	; Load segments into STAR MSR
	mov		rcx, 0xc0000081
	rdmsr
	mov		edx, 0x00180008
	wrmsr
	ret

syscall_entry:
	o64	sysret
	
	ret
	hlt
	cli

	;call TSSGetStack
	;mov rsp, rax
	; Save and switch context
	SYSCALL_SAVE
	push	r11
	push	rcx

	; Get syscall handler and call the routine
	
	

	;call	SyscallEntry

	; Restore state-sensitive information and exit
    
	pop		rcx
	pop		r11
    SYSCALL_RESTORE

    sti

    o64	sysret
	
	ret

