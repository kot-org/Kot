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
	push	rdx
	push	rbx
%endmacro
%macro	SYSCALL_RESTORE	0
	pop	rbx
	pop	rdx
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
	cli
	mov rdi, rsp ; save rsp
	push r11 ; rflag
	push rcx ; rip

	; Save and switch context
	SYSCALL_SAVE
	

	; Get syscall handler and call the routine	

	call SyscallEntry

	; Restore state-sensitive information and exit    
	
    SYSCALL_RESTORE

	pop	rcx ; rip
	pop	r11 ; rflags
	sti
	o64 sysret

	push rsi ; push ss
	push rdi ; push stack
	push r11 ; push rflags
	push rax ; push cs
	push rcx ; push rip

	
    iretq

