[BITS 64]
GLOBAL atomicLock, atomicUnlock


atomicLock:	
	lock bts	qword [rdi], 0
	mov			rcx, 1
	mov			rax, 0
	cmovnc		rax, rcx
	ret	

atomicUnlock:	
	lock  btr		qword [rdi], 0
	mov			rcx, 1
	mov			rax, 1
	cmovnc		rax, rcx
	ret	