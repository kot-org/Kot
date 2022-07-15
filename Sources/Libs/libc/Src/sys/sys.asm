GLOBAL DoSyscall

DoSyscall:
    push    rbp
    mov     rbp, rsp
    mov     rax, rdi
    mov     rdi, rsi
    mov     rsi, rdx
    mov     rdx, rcx
    mov     r10, r8
    mov     r8, qword[rbp + 0x10]
    syscall
    pop     rbp
    ret