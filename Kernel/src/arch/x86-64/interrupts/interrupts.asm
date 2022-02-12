[bits 64]

GLOBAL InterruptEntryList
EXTERN InterruptHandler

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
    add    rsp, 16  ; remove error code and interrupt number from stack 
%endmacro

%macro CREAT_INTERRUPT_NAME 1  

    dq EntryInterruptHandler%1

%endmacro

; function

%macro GLOBAL_INTERRUPT_HANDLER 0
    swapgs
    PUSH_REG

    mov rdi, rsp
    mov rsi, [gs:0x0]

    call InterruptHandler

    mov rax, [rsp + 0xA8]      ; ss
    mov qword [gs:0x18], rax
    mov rax, [rsp + 0x90]      ; cs
    mov qword [gs:0x20], rax

    POP_REG

    swapgs
    iretq

%endmacro

; entries

%macro INTERRUPT_WITHOUT_ERROR_CODE  1

EntryInterruptHandler%1:
    push 0  ; fill the stack
    push %1
    GLOBAL_INTERRUPT_HANDLER

%endmacro

%macro INTERRUPT_WITH_ERROR_CODE  1

EntryInterruptHandler%1:
    push %1
    GLOBAL_INTERRUPT_HANDLER

%endmacro

; creat functions

INTERRUPT_WITHOUT_ERROR_CODE 0
INTERRUPT_WITHOUT_ERROR_CODE 1
INTERRUPT_WITHOUT_ERROR_CODE 2
INTERRUPT_WITHOUT_ERROR_CODE 3
INTERRUPT_WITHOUT_ERROR_CODE 4
INTERRUPT_WITHOUT_ERROR_CODE 5
INTERRUPT_WITHOUT_ERROR_CODE 6
INTERRUPT_WITHOUT_ERROR_CODE 7
INTERRUPT_WITH_ERROR_CODE   8
INTERRUPT_WITHOUT_ERROR_CODE 9
INTERRUPT_WITH_ERROR_CODE   10
INTERRUPT_WITH_ERROR_CODE   11
INTERRUPT_WITH_ERROR_CODE   12
INTERRUPT_WITH_ERROR_CODE   13
INTERRUPT_WITH_ERROR_CODE   14
INTERRUPT_WITHOUT_ERROR_CODE 15
INTERRUPT_WITHOUT_ERROR_CODE 16
INTERRUPT_WITH_ERROR_CODE   17
INTERRUPT_WITHOUT_ERROR_CODE 18
INTERRUPT_WITHOUT_ERROR_CODE 19
INTERRUPT_WITHOUT_ERROR_CODE 20
INTERRUPT_WITHOUT_ERROR_CODE 21
INTERRUPT_WITHOUT_ERROR_CODE 22
INTERRUPT_WITHOUT_ERROR_CODE 23
INTERRUPT_WITHOUT_ERROR_CODE 24
INTERRUPT_WITHOUT_ERROR_CODE 25
INTERRUPT_WITHOUT_ERROR_CODE 26
INTERRUPT_WITHOUT_ERROR_CODE 27
INTERRUPT_WITHOUT_ERROR_CODE 28
INTERRUPT_WITHOUT_ERROR_CODE 29
INTERRUPT_WITH_ERROR_CODE   30
INTERRUPT_WITHOUT_ERROR_CODE 31


%assign i 32
%rep 256 - i
    INTERRUPT_WITHOUT_ERROR_CODE i
%assign i i+1
%endrep

; link functions

InterruptEntryList:
    %assign i 0
    %rep 256
        CREAT_INTERRUPT_NAME i
    %assign i i+1
    %endrep

