[BITS 64]

%include "Src/arch/x86-64/cpu/cpu.inc"

GLOBAL AtomicLock, AtomicRelease, AtomicAquire

%macro	CF_RESULT	0
	mov		rcx, 1
	mov		rax, 0
	cmovnc	rax, rcx
%endmacro

AtomicLock:		; rdi = mutex location memory
	lock bts		QWORD [rdi], 0x0
	CF_RESULT		
	ret

AtomicRelease:	; rdi = mutex location memory , 0x0 = location of the bit where we store the statu
	lock btr		QWORD [rdi], 0x0
	CF_RESULT
	ret

AtomicAquire:		; rdi = mutex location memory , 0x0 = location of the bit where we store the statu
	.acquire:
		lock bts	QWORD [rdi], 0x0
		jnc			.exit				; CF = 0 to begin with
	.spin:
		int 		0x40
		bt			QWORD [rdi], 0x0
		jc			.spin				; CF = 1 still
		jmp			.acquire
	.exit:
		ret