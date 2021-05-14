[bits 64]

GLOBAL isr_default_handler
GLOBAL __enable_irq
GLOBAL __disable_irq
GLOBAL __getflags
GLOBAL __idle_thread_wait_loop
GLOBAL interrupt_yield
GLOBAL yield_irq_handler
GLOBAL isr_handler0
GLOBAL isr_handler1
GLOBAL isr_handler2
GLOBAL isr_handler3
GLOBAL isr_handler4
GLOBAL isr_handler5
GLOBAL isr_handler6
GLOBAL isr_handler7
GLOBAL isr_handler8
GLOBAL isr_handler9
GLOBAL isr_handler10
GLOBAL isr_handler11
GLOBAL isr_handler12
GLOBAL isr_handler13
GLOBAL isr_handler14
GLOBAL isr_handler15
GLOBAL isr_handler16
GLOBAL isr_handler17
GLOBAL isr_handler18
GLOBAL isr_handler19

isr_default_handler:
    iretq

__enable_irq:
    sti
    ret

__disable_irq:
    cli
    ret

__getflags:
    pushfq
    pop rax
    ret

__idle_thread_wait_loop:
	hlt
	jmp __idle_thread_wait_loop

interrupt_yield:
	int 0x81
	ret

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
    mov r9, [rsp-0x38]
    mov r8, [rsp-0x40]
    mov rdi, [rsp-0x48]
    mov rsi, [rsp-0x50]
    mov rbp, [rsp-0x58]
    mov rbx, [rsp-0x68]
    mov rdx, [rsp-0x70]
    mov rcx, [rsp-0x78]
    mov rax, [rsp-0x80]
%endmacro

extern interrupt_handle
IsrCommon:
    cli
    PUSHAQ

    mov rdi, rsp
    call interrupt_handle

    POPAQ
    sti
    add rsp, 16
    iretq

isr_handler0:
	push qword 0
	push qword 0
	jmp IsrCommon

isr_handler1:
	push qword 0
	push qword 1
	jmp IsrCommon

isr_handler2:
	push qword 0
	push qword 2
	jmp IsrCommon

isr_handler3:
	push qword 0
	push qword 3
	jmp IsrCommon

isr_handler4:
	push qword 0
	push qword 4
	jmp IsrCommon

isr_handler5:
	push qword 0
	push qword 5
	jmp IsrCommon

isr_handler6:
	push qword 0
	push qword 6
	jmp IsrCommon

isr_handler7:
	push qword 0
	push qword 7
	jmp IsrCommon

isr_handler8:
	push qword 8
	jmp IsrCommon

isr_handler9:
	push qword 0
	push qword 9
	jmp IsrCommon

isr_handler10:
	push qword 10
	jmp IsrCommon

isr_handler11:
	push qword 11
	jmp IsrCommon

isr_handler12:
	push qword 12
	jmp IsrCommon

isr_handler13:
	push qword 13
	jmp IsrCommon

isr_handler14:
	push qword 14
	jmp IsrCommon

isr_handler15:
	push qword 0
	push qword 15
	jmp IsrCommon

isr_handler16:
	push qword 0
	push qword 16
	jmp IsrCommon

isr_handler17:
	push qword 17
	jmp IsrCommon

isr_handler18:
	push qword 0
	push qword 18
	jmp IsrCommon

isr_handler19:
	push qword 0
	push qword 19
	jmp IsrCommon

extern pic_eoi
extern task_switch
yield_irq_handler:
	cli

	PUSHAQ

	mov rdi, rsp
	call task_switch

	mov rsp, rax
	mov cr3, rdx

	mov rdi, 0
	call pic_eoi

	POPAQ

	sti
	iretq

