GLOBAL DoSyscall

DoSyscall:
    mov [rbp-4], rdi
    mov [rbp-8], rsi
    mov [rbp-12], rdx
    mov [rbp-16], rcx
    mov [rbp-20], r8
    mov [rbp-24], r9
     
    mov rax, [rbp-4]
    mov rdi, [rbp-8]
    mov rsi, [rbp-12]
    mov rdx, [rbp-16]
    mov r10, [rbp-20]
    mov r8, [rbp-24]
    mov r9, [rbp+16]
    int 0x80
    ret