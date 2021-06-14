[BITS 16]

GLOBAL Trampoline

%define CODE_SEG     0x0008
%define DATA_SEG     0x0010
%define Target(addr) ((addr - Trampoline) + 0x8000)

;------------------------------Code-----------------------------------

Trampoline: 
    cli
    cld
    
    mov	byte [Target(DataTrampoline.Status)], 1
    
    ;Cr4 physical address extension
    mov eax, cr4
    or eax, 1 << 5
    mov cr4, eax

    ;Load Cr3
    mov eax, [Target(DataTrampoline.Paging)]
    mov cr3, eax

    ;efer long mode enable
    mov ecx, 0xC0000080 
    rdmsr
    or eax, 1 << 8
    wrmsr
    
    ;Cr0 protected mode enable & paging
    mov eax, cr0
    or eax, 0x80000001
    mov cr0, eax    

    lgdt [Target(GDT.Pointer)]

    jmp CODE_SEG:Target(TrampolineLongMode)
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
    mov ax, DATA_SEG
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    mov rsp, qword [Target(DataTrampoline.Stack)]

    mov rax, cr0
    and ax, 0xFFFB      
    or ax, 0x2          
    mov cr0, rax

    mov rax, cr4
    or ax, 3 << 9		
    mov cr4, rax

    mov    rax, 1
    cpuid
    shr    rbx, 24
    mov    rdi, rbx

    mov	byte [Target(DataTrampoline.Status)], 3

    jmp [Target(DataTrampoline.MainEntry)]
    hlt


;------------------------------Data-----------------------------------
[BITS 64] 

GLOBAL DataTrampoline

DataTrampoline:
	.Status:			        db	0
    .GDTPointer:                dq  0
    .Paging:                    dq  0
    .Stack:                     dq  0
    .MainEntry:                 dq  0    

times 4096 - ($ - $$) db 0