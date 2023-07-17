[bits 64]
GLOBAL LoadGDT

LoadGDT:  
    lgdt [rdi]
    mov ax, 0x10 ; ss kernel
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov fs, ax
    mov gs, ax
    pop rdi
    mov rax, 0x08 ; cs kernel
    push rax
    push rdi
    retfq