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
    mov rcx, 0xc0000080 ; EFER MSR
    rdmsr               ; read current EFER
    or eax, 1           ; enable SCE bit
    wrmsr               ; write back new EFER
    mov rcx, 0xc0000081 ; STAR MSR
    rdmsr               ; read current STAR
    mov edx, 0x00180008 ; load up GDT segment bases 0x0 (kernel) and 0x18 (user)
    wrmsr               ; write back new STAR
    ret                 ; return back to C

syscall_entry:
	; Save and switch context
	PUSH_REG
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
		POP_REG	
		o64 sysret	
		ret
	.kernel_exit:
		POP_REG
		mov		rdi, 0
		call	TSSGetStack		
		mov		rsp, rax
		ret

