.global atomicLock
.global atomicUnlock
.global atomicAcquire

CF_RESULT:
    mov $1, %rcx
    mov $0, %rax
    cmovnc %rcx, %rax
    ret

atomicLock:
    lock btsq %rsi, (%rdi)
    call CF_RESULT
    ret

atomicUnlock:
    btrq %rsi, (%rdi)
    call CF_RESULT
    ret

atomicAcquire:
.acquire:
    lock btsq %rsi, (%rdi)
    jnc .exit
.spin:
    pause
    btq %rsi, (%rdi)
    jc .spin
    jmp .acquire
.exit:
    ret
