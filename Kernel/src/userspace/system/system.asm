[bits 64]

%macro    PUSH_REG    0
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
%macro    POP_REG        0
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


GLOBAL EnableSystemCall
EXTERN TSSGetStack, SyscallEntry, SystemExit


EnableSystemCall:
	mov		rax, syscall_entry
	mov		rdx, rax
	shr		rdx, 0x20
	mov		rcx, 0xc0000082
	wrmsr
	mov		rcx, 0xc0000080
	rdmsr
	or		eax, 1
	wrmsr
	mov		rcx, 0xc0000081
	rdmsr
	mov		edx, 0x00180008
	wrmsr
	ret      

syscall_entry:
	cli

	; Save and switch context
	PUSH_REG
	push	r11
	push	rcx
 
	; Get syscall handler and call the routine
	mov		r14, rax
	mov 	r15, r10

	call	SyscallEntry

	; Restore state-sensitive information and exit
	pop		rcx
	pop		r11
    
    POP_REG
	.sysret_exit:
		o64	sysret
		sti
		ret
	.kernel_exit:
		mov		rdi, 0
		call	TSSGetStack		
		mov		rsp, rax
		sti
		ret

