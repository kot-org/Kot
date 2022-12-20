GLOBAL atomicLock, atomicUnlock, atomicAcquire

%macro	CF_RESULT	0
	mov		rcx, 1
	mov		rax, 0
	cmovnc	rax, rcx
%endmacro

atomicLock:	
	lock bts	QWORD [rdi], rsi
	CF_RESULT		
	ret

atomicUnlock:	
	btr		QWORD [rdi], rsi
	CF_RESULT
	ret

atomicAcquire:	
	.acquire:
		lock bts	QWORD [rdi], rsi
		jnc			.exit				
	.spin:
		pause
		bt			QWORD [rdi], rsi
		jc			.spin				
		jmp			.acquire
	.exit:
		ret

