%macro    PUSH_REG    0
    push    r15
    push    r14
    push    r13
    push    r12
    push    r11
    push    r10
    push    r9
    push    r8
    push    rbp
    push    rdi
    push    rsi
    push    rdx
    push    rcx
    push    rbx
    push    rax
    push   qword [gs:0x8]
    mov     rax, cr3
    push    rax
%endmacro

%macro    POP_REG        0
    pop    rax
    mov    cr3, rax
    pop    qword [gs:0x8]
    pop    rax
    pop    rbx
    pop    rcx
    pop    rdx
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
%endmacro

%macro SWAPGS_IF_NECESSARY 1 
    cmp qword [rsp + 0x18], 0x8 ; 0x8 kernel cs
	jnz DO_SWAPGS%1
    jmp NO_SWAPGS%1
DO_SWAPGS%1:
	swapgs
NO_SWAPGS%1:
%endmacro
