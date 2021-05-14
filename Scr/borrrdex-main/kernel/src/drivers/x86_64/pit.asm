[bits 64]

GLOBAL __pit_irq_handler
GLOBAL __enable_apic
extern pit_counter
extern pic_eoi

__pit_irq_handler:
    cli

    mov [rsp-0x8], r15
    mov [rsp-0x10], r14
    mov [rsp-0x18], r13
    mov [rsp-0x20], r12
    mov [rsp-0x28], r11
    mov [rsp-0x30], r10
    mov [rsp-0x38], r9
    mov [rsp-0x40], r8
    mov [rsp-0x48], rdi
    mov [rsp-0x50], rsi
    mov [rsp-0x58], rbp
    mov [rsp-0x60], rsp
    mov [rsp-0x68], rbx
    mov [rsp-0x70], rdx
    mov [rsp-0x78], rcx
    mov [rsp-0x80], rax
    sub rsp, 0x80

    xor rax, rax
    mov eax, pit_counter
    add [rax], dword 1

    mov rdi, 0
    call pic_eoi

    add rsp, 0x80
    mov r15, [rsp-0x8]
    mov r14, [rsp-0x10]
    mov r13, [rsp-0x18]
    mov r12, [rsp-0x20]
    mov r11, [rsp-0x28]
    mov r10, [rsp-0x30]
    mov r9, [rsp-0x38]
    mov r8, [rsp-0x40]
    mov rdi, [rsp-0x48]
    mov rsi, [rsp-0x50]
    mov rbp, [rsp-0x58]
    mov rbx, [rsp-0x68]
    mov rdx, [rsp-0x70]
    mov rcx, [rsp-0x78]
    mov rax, [rsp-0x80]

    sti
    iretq

__enable_apic:
    push rbp
    mov rbp, rsp

    mov ecx, 0x1B
    rdmsr
    or eax, 0x800
    wrmsr

    mov rsp, rbp
    pop rbp
    ret