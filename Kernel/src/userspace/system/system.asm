[bits 64]
ALIGN	4096

%macro    SYSCALL_SAVE    0
    push    r15
    push    r14
    push    r13
    push    r12
    push    r11
    push    r10
    push    r9
    push    r8
    push    rbp
    push    rdi
    push    rsi
    push    rsp
    push    rdx
    push    rcx
    push    rbx
    push    rax
%endmacro
%macro    SYSCALL_RESTORE        0
    pop    rax
    pop    rbx
    pop    rcx
    pop    rdx
    pop    rsp
    pop    rsi
    pop    rdi
    pop    rbp
    pop    r8
    pop    r9
    pop    r10
    pop    r11
    pop    r12
    pop    r13
    pop    r14
    pop    r15
%endmacro


GLOBAL EnableSystemCall, syscall_entry
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
    swapgs

	mov rdi, rsp ; save rsp

	; Save and switch context
	SYSCALL_SAVE	

    ; Get core id
    mov    rax, 1
    cpuid
    shr    rbx, 24
    mov    rsi, rbx

	; Get syscall handler and call the routine

	mov rdi, rsp
	call SyscallEntry

	; Restore state-sensitive information and exit    
	
    SYSCALL_RESTORE
    
    push r9 ; push ss
	push rdi ; push stack
	push r11 ; push rflags
	push r8 ; push cs
	push rcx ; push rip

    swapgs
    
    sti
	iretq

