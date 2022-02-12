GLOBAL DoSyscall
DoSyscall:
    push    rbp
    mov     rbp, rsp
    mov     rax, rdi
    mov     rdi, rsi
    mov     rsi, rdx
    mov     rdx, rcx
    mov     r10, r8
    mov     r8, r9
    mov     r9, qword[rbp+16]
    syscall
    mov     rax, rdi
    pop     rbp
    ret