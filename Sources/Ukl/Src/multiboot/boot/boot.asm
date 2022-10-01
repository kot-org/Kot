[BITS 32]

%define CODE_SEG     0x0008
%define DATA_SEG     0x0010

GLOBAL boot_kernel

boot_kernel:
    mov ebp, esp
    cli
    cld
    
    ;long mode enable efer
    mov ecx, 0xC0000080 
    rdmsr
    or eax, 1 << 8
    wrmsr
    
    ;Load Cr3
    mov eax, [ebp+4]
    mov cr3, eax

    ;Cr4 physical address extension
    mov eax, cr4
    or eax, 1 << 5
    mov cr4, eax

    ;Cr0 enable paging
    mov eax, cr0
    or eax, 1 << 31
    mov cr0, eax

    lgdt [GDT.Pointer]

    jmp CODE_SEG:TrampolineLongMode
    hlt

GDT:
.Null:
    dq 0x0000000000000000             
 
.Code:
    dq 0x00209A0000000000             
    dq 0x0000920000000000             
 
ALIGN 4
    dw 0                             

.Pointer:
    dw $ - GDT - 1             
    dd GDT                        

[BITS 64]

TrampolineLongMode:
    mov rsi, [ebp+8] ; entry point
    mov rcx, [ebp+16] ; stack
    mov rdi, [ebp+24] ; argument 0

    mov ax, DATA_SEG
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    mov rsp, rcx

    mov rax, cr0
    and ax, 0xFFFB      
    or ax, 0x2          
    mov cr0, rax

    mov rax, cr4
    or ax, 3 << 9		
    mov cr4, rax

    jmp rsi
    hlt