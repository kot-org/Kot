[bits 64]

GLOBAL __load_gdt


__load_gdt:
    lgdt [rdi]

    ; This should probably be done earlier in the boot process
    ; Load KernelData into the Data Segment registers
    mov rax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    ; Load KernelCode into the Code Segment registers
    ; (Using side effect of retfq)
    pop rdi
    mov rax, 0x08
    push rax
    push rdi
    retfq