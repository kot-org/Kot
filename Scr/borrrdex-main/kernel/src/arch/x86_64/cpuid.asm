[bits 64]
GLOBAL _cpuid

_cpuid:
    push rbp
    mov rbp, rsp

    push rbx
    push rcx
    push rdx

    mov rax, [rdi]
    cpuid
    mov [rdi], rax
    mov [rsi], rbx

    mov rax, rdx
    pop rdx
    mov rbx, rcx
    pop rcx

    mov [rcx], rax
    mov [rdx], rbx

    pop rbx

    mov rsp, rbp
    pop rbp
    ret