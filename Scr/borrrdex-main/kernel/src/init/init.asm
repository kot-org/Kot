[bits 16]
global ap_trampoline

ap_trampoline:
    cli
    cld
    jmp 0h:8040h
    align 16

_L8010_GDT_table:
    dq 0
    dd 0x0000ffff, 0x00cf9a00
    dd 0x0000ffff, 0x008f9200 
    dd 0x00000068, 0x00cf8900 
_L8030_GDT_value:
    dw _L8030_GDT_value - _L8010_GDT_table - 1
    dd 0x8010
    dq 0
    align 64

_L8040:
    xor ax, ax
    mov ds, ax
    lgdt [0x8030]
    mov eax, cr0
    or  eax, 1
    mov cr0, eax
    jmp 0x8:0x8060
    align 32

[bits 32]
extern ap_startup
extern aprunning
extern bspdone
extern stack_top
extern pml4_address
extern gdt_addr
extern idt_addr

_L8060:
    mov ax, 16
    mov ds, ax
    mov ss, ax

    mov eax, 1
    cpuid
    shr ebx, 24
    mov edi, ebx

    shl ebx, 15
    mov esp, dword [stack_top]
    sub esp, ebx
    push edi

loop:  
    pause
    cmp [bspdone], byte 0
    jz loop
    lock inc byte [aprunning]

    mov eax, 0xFF
    out 0xA1, al
    out 0x21, al
    nop
    nop

    mov eax, [idt_addr]
    lidt [eax]

    mov eax, 10100000b
    mov cr4, eax

    mov edx, dword [pml4_address]
    mov cr3, edx

    mov ecx, 0xC0000080
    rdmsr
    or eax, 0x00000100
    wrmsr

    mov ebx, cr0
    or ebx, 0x80000001
    mov cr0, ebx

    mov eax, [gdt_addr]
    lgdt [eax]

    jmp dword 0x8:ap_startup
