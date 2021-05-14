[bits 64]

GLOBAL __spinlock_acquire

__spinlock_acquire:
    push rax
    push rbx
    mov rax, 0x1
    mov rbx, rdi

spinlock_loop:
    xchg eax, [ebx]
    test rax, rax
    jnz spinlock_loop

    pop rbx
    pop rax
    ret