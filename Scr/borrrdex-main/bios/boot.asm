[bits 32]
[section .boot]

%define PDE_ADDRESS     0x64000
%define PDPTE_ADDRESS   0x62000
%define PML4_ADDRESS    0x60000

%define PAGE_PRESENT    0x1
%define PAGE_WRITE      0x2
%define PAGE_2MB        0x80

%define PAGE_FLAGS      (PAGE_PRESENT | PAGE_WRITE | PAGE_2MB)

MultibootHeader:
    dd 0x1badb002
    dd 2
    dd -(0x1badb002 + 2)
    dd 0
    dd 0
    dd 0
    dd 0
    dd 0

[section .text]
GLOBAL _start

align 16
TempGDT32:
    dq 0x0000000000000000
    dq 0x00cf9a000000ffff
    dq 0x00cf92000000ffff

align 16
TempGDT64:
    dq 0x0000000000000000
    dq 0x00a09a0000000000
    dq 0x00a0920000000000

align 16
GDT32:
    dw 23
    dd TempGDT32

align 16
GDT64:
    dw 23
    dq TempGDT64

_start:
    cli

    push dword 0
    push eax
    push dword 0
    push ebx

    lgdt [GDT32]

    jmp 0x8:.1

    .1:
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov gs, ax
    mov fs, ax
    mov ss, ax

    xor eax, eax
    xor ebx, ebx
    xor ecx, ecx
    xor edx, edx

    mov eax, PDE_ADDRESS
    mov [eax], dword (1024 * 1024 * 0 + PAGE_FLAGS)
    mov [eax + 4], dword 0
    mov [eax + 8], dword (1024 * 1024 * 2 + PAGE_FLAGS)
    mov [eax + 12], dword 0
    mov [eax + 16], dword (1024 * 1024 * 4 + PAGE_FLAGS)
    mov [eax + 20], dword 0
    mov [eax + 24], dword (1024 * 1024 * 6 + PAGE_FLAGS)
    mov [eax + 28], dword 0
    mov [eax + 32], dword (1024 * 1024 * 8 + PAGE_FLAGS)
    mov [eax + 36], dword 0
    mov [eax + 40], dword (1024 * 1024 * 10 + PAGE_FLAGS)
    mov [eax + 44], dword 0
    mov [eax + 48], dword (1024 * 1024 * 12 + PAGE_FLAGS)
    mov [eax + 52], dword 0
    mov [eax + 56], dword (1024 * 1024 * 14 + PAGE_FLAGS)
    mov [eax + 60], dword 0
    mov [eax + 64], dword (1024 * 1024 * 16 + PAGE_FLAGS)
    mov [eax + 68], dword 0
    mov [eax + 72], dword (1024 * 1024 * 18 + PAGE_FLAGS)
    mov [eax + 76], dword 0

    or eax, dword 3
    mov ebx, PDPTE_ADDRESS
    mov [ebx], eax
    mov [ebx + 4], dword 0

    or ebx, dword 3
    mov eax, PML4_ADDRESS
    mov [eax], ebx
    mov [eax + 4], dword 0
    mov [eax + 8], ebx
    mov [eax + 12], dword 0

    mov cr3, eax

    mov eax, cr4
    or eax, 0x000000B0
    mov cr4, eax

    mov ecx, 0xC0000080
    rdmsr
    or eax, 0x00000101
    wrmsr

    mov eax, cr0
    or eax, 0x80000000
    mov cr0, eax

    lgdt [GDT64]

    jmp 0x8:.2

    hlt

    .2:
    [bits 64]
    extern init
    extern init_stack

    mov eax, esp
    xor rsp, rsp
    mov esp, eax

    mov rax, [init_stack]
    o64 mov rbx, StackStart
    mov [rax], rbx

    pop rbx
    pop rax

    o64 mov rcx, StackStart
    mov rsp, rcx
    mov rbp, rcx

    mov rdi, rax
    mov rsi, rbx
    call init

    .5:
    hlt
    jmp .5

align 16
StackEnd:
    times 0x1000 db 0
StackStart: