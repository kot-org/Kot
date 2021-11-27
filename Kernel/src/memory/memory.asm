;GLOBAL memcpy

memcpy:
    mov rax, rdi
    mov rcx, rdx
    rep movsb
    ret