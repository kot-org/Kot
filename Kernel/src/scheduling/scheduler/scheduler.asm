[bits 64]

%macro    POP_REG        0
    pop    rax
    pop    rbx
    pop    rcx
    pop    rdx
    pop    rsp
    pop    rsi
    pop    rdi
    pop    rbp
    pop    r8
    pop    r9
    pop    r10
    pop    r11
    pop    r12
    pop    r13
    pop    r14
    pop    r15
    pop    gs
    pop    fs
%endmacro

EXTERN TSSGetStack
GLOBAL LoadTask, LoadKernel

LoadTask:
    pop rax
    push rax
    ret

LoadKernel:
    mov rdi, 0
	call TSSGetStack   
    mov	rsp, rax
	ret