[bits 64]

%macro PUSHAQ 0
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
%endmacro

%macro POPAQ 0
    add rsp, 0x80
    mov r15, [rsp-0x8]
    mov r14, [rsp-0x10]
    mov r13, [rsp-0x18]
    mov r12, [rsp-0x20]
    mov r11, [rsp-0x28]
    mov r10, [rsp-0x30]
    mov r9,  [rsp-0x38]
    mov r8,  [rsp-0x40]
    mov rdi, [rsp-0x48]
    mov rsi, [rsp-0x50]
    mov rbp, [rsp-0x58]
    mov rbx, [rsp-0x68]
    mov rdx, [rsp-0x70]
    mov rcx, [rsp-0x78]
    mov rax, [rsp-0x80]
%endmacro

GLOBAL syscall_irq_handler
extern syscall_handle
extern tss_get
extern process_finish

syscall_irq_handler:
    cli

    PUSHAQ

    mov rdi, 0
    call tss_get
    mov rdi, rsp
    mov rsp, qword [rax + 0x04]
    push rdi

    call syscall_handle
    pop rsp
    POPAQ

    cmp rdi, 0x201
    je .kernel_exit

    sti
    o64 sysret
    .kernel_exit:
        mov rdi, 0
        call tss_get
        mov rsp, qword [rax + 0x04]
        mov rdi, rsi
        sti
        push $process_finish
        ret


GLOBAL __syscall_setup

__syscall_setup:
    mov rax, $syscall_irq_handler
    mov r8, rax
    shr r8, 0x20
    mov rdx, r8

    ; Truncate to 32-bits
    mov eax, eax
    mov edx, edx

    ; LSTAR MSR, set handler RIP
    mov rcx, 0xc0000082
    wrmsr

    ; While we are here, set up syscall / sysret
    mov rcx, 0xc0000080
    rdmsr
    or eax, 1
    wrmsr
    mov rcx, 0xc0000081
    rdmsr
    mov edx, 0x00180008
    wrmsr

    ret