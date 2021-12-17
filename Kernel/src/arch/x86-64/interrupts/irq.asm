EXTERN ExternIRQFunction

[BITS 64]

cr3Savers:			        dw	0
rspSaver:                   dw  0

ExternIRQFunction:
    mov rax, cr3
    push rax
    mov cr3, rsi
    mov rbx, rsp
    mov rsp, rdi
    push rbx

    call rdx

    pop rsp
    pop rax
    mov cr3, rax
    ret

