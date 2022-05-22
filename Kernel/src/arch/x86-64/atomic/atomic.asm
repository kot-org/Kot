[BITS 64]
GLOBAL atomicLock, atomicUnlock, atomicAcquire

%macro	CF_RESULT	0
	mov		rcx, 1
	mov		rax, 0
	cmovnc	rax, rcx
%endmacro

atomicLock:	
	lock bts		QWORD [rdi], rsi
	CF_RESULT		
	ret

atomicUnlock:	; rdi = mutex location memory , rsi = location of the bit where we store the statu
	lock btr		QWORD [rdi], rsi
	CF_RESULT
	ret

atomicAcquire:		; rdi = mutex location memory , rsi = location of the bit where we store the statu
	.acquire:
		lock bts	QWORD [rdi], rsi
		jnc			.exit				; CF = 0 to begin with
	.spin:
		pause
		bt			QWORD [rdi], rsi
		jc			.spin				; CF = 1 still
		jmp			.acquire
	.exit:
		ret

