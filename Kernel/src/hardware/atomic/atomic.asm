[BITS 64]
GLOBAL atomicLock, atomicUnlock, atomicWait, atomicCheck

atomicLock:
    mov    rax, 1
    cpuid
    shr    rbx, 24
    mov    rax, rbx
    lock xchg qword [rdi], rax
    ret

atomicUnlock:
	mov rax, 0
    lock xchg qword [rdi], rax
    ret

atomicWait:
	mov rax, 0
    lock cmpxchg qword [rdi], rax ; if [rdi] == 0
    je                	.exit
    jmp                	.spin
    .spin:
        pause
        mov rax, 0
        lock cmpxchg     qword [rdi], rax ; if [rdi] == 0
        je            	.exit                
        jmp            	.spin
    .exit:
        ret

atomicCheck:
    mov    rax, 1
    cpuid
    shr    rbx, 24
    mov    rax, rbx
    lock cmpxchg qword [rdi], rax ; if [rdi] == coreID
    je .true
    jmp .false
    .true:   
        mov rax, 1                 
        ret
    .false:
        mov rax, 0
        ret

