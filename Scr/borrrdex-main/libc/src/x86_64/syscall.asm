[bits 64]

GLOBAL _syscall

_syscall:
    ; We are about to clobber RCX, move it somewhere else
    push rbp
    mov rbp, rsp

    mov r8, rcx

    syscall

    leave
    ret
