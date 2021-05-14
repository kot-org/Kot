[bits 64]

GLOBAL Entry_PageFault_Handler, Entry_DoubleFault_Handler, Entry_GPFault_Handler, Entry_KeyboardInt_Handler, Entry_MouseInt_Handler, Entry_PITInt_Handler
EXTERN PageFault_Handler, DoubleFault_Handler, GPFault_Handler, KeyboardInt_Handler, MouseInt_Handler, PITInt_Handler

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
%endmacro
%macro    POP_REG        0
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


Entry_PageFault_Handler:
	cli
    PUSH_REG
    call PageFault_Handler
    POP_REG
	sti
    ret

Entry_DoubleFault_Handler:
	cli
    PUSH_REG
    call DoubleFault_Handler
    POP_REG
	sti
    ret

Entry_GPFault_Handler:
	cli
    PUSH_REG
    call GPFault_Handler
    POP_REG
	sti
    ret

Entry_KeyboardInt_Handler:
	cli
    PUSH_REG
    call KeyboardInt_Handler
    POP_REG
	sti
    iretq 

Entry_MouseInt_Handler:
	cli
    PUSH_REG
    call MouseInt_Handler
    POP_REG
	sti
    iretq 

Entry_PITInt_Handler:
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
    add rsp, 0x80
    
    mov rdi, $
	
    call PITInt_Handler   
    CMP rax, 0 	
    JNZ SwitchTask

    sub rsp, 0x80
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
    mov rsp, [rsp-0x60]
    mov rbx, [rsp-0x68]
    mov rdx, [rsp-0x70]
    mov rcx, [rsp-0x78]
    mov rax, [rsp-0x80]

	sti

    ret 

SwitchTask:
    sub rsp, 0x80
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
    mov rsp, [rsp-0x60]
    mov rbx, [rsp-0x68]
    mov rdx, [rsp-0x70]
    mov rcx, [rsp-0x78]

    ;mov [r15-0x8], rax
    
    ;mov rax, [rsp-0x80]

    sti
    jmp rax
    ret