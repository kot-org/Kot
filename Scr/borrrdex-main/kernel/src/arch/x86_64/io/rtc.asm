[bits 64]
GLOBAL _rtc_init_interrupt
GLOBAL _rtc_set_interrupt_frequency
GLOBAL __rtc_irq_handler
extern rtc_handle

__rtc_irq_handler:
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

    call rtc_handle

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

_rtc_init_interrupt:
    cli
    mov al, 0x8B
    out 0x70, al
    in al, 0x71
    push rax
    mov al, 0x8B
    out 0x70, al
    pop rax
    or al, 0x40
    out 0x71, al
    sti
    ret

_rtc_set_interrupt_frequency:
    cli
    mov al, 0x8A
    out 0x70, al
    in al, 0x71
    push rax
    mov al, 0x8A
    out 0x70, al
    pop rax
    and al, 0xF0
    and di, 0x0F
    or ax, di
    out 0x71, al
    sti
    ret
