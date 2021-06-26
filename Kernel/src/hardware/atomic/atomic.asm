[BITS 64]
GLOBAL atomicLock, atomicUnlock, atomicWait

atomicLock:
    mov rax, 1
    lock xchg qword [rdi], rax
    ret

atomicUnlock:
	mov qword [rdi], 0
    ret

atomicWait:
	mov rax, 1
    lock cmpxchg qword [rdi], rax ; if [rdi] != 0
    je                	.spin
    jmp                	.exit
    .spin:
        pause
        lock cmpxchg     QWORD [rdi], rax ; if [rdi] != 0
        je            	.spin                
        jmp            	.exit
    .exit:
        ret

atomicLoker:
    ret