[BITS 64]

%include "Src/arch/x86-64/cpu/cpu.inc"

EXTERN SaveTask
GLOBAL atomicLockAsm, atomicUnlockAsm, atomicAcquireAsm, JumpToKernelTaskAsm, SaveTaskAsm, RestoreTaskAsm

%macro	CF_RESULT	0
	mov		rcx, 1
	mov		rax, 0
	cmovnc	rax, rcx
%endmacro

atomicLockAsm:		; rdi = mutex location memory
	lock bts		QWORD [rdi], rsi
	CF_RESULT		
	ret

atomicUnlockAsm:	; rdi = mutex location memory , rsi = location of the bit where we store the statu
	lock btr		QWORD [rdi], rsi
	CF_RESULT
	ret

atomicAcquireAsm:		; rdi = mutex location memory , rsi = location of the bit where we store the statu
	.acquire:
		lock bts	QWORD [rdi], rsi
		jnc			.exit				; CF = 0 to begin with
	.spin:
		int 		0x40
		bt			QWORD [rdi], rsi
		jc			.spin				; CF = 1 still
		jmp			.acquire
	.exit:
		ret

JumpToKernelTaskAsm:
	cli
	mov rax, [rdi + 0x8]
	mov rdi, qword [gs:0x8]
	mov rdi, [rdi + 0x18]
	mov [rax], rdi
    mov rsp, rdi
    POP_REG
	iretq

SaveTaskAsm:
	mov rdi, qword [gs:0x8]
	mov rdi, [rdi + 0x18]
	mov rdi, [rdi + 0x56]

	PUSH_REG
	mov rdi, rsp
	call SaveTask
	ret

RestoreTaskAsm:
	mov rsp, rdi
    POP_REG
	iretq