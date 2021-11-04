[BITS 64]

GLOBAL IdleTask

IdleTask:
    mov rax, 0
    mov rdi, 0
    mov rsi, 0
    mov rdx, 0
    mov r10, 0
    mov r8, 0
    mov r9, 0
    int 0x80
    jmp IdleTask

times 4096 - ($ - $$) db 0