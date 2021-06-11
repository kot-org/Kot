[BITS 16]  
GLOBAL Trampoline
EXTERN TrampolineEnd

%define PAGE_PRESENT    (1 << 0)
%define PAGE_WRITE      (1 << 1)
 
%define CODE_SEG     0x0008
%define DATA_SEG     0x0010


;------------------------------Code-----------------------------------

Trampoline: 
    cli
    cld
    mov	byte [DataTrampoline.Status], 1
    ;Cr0 paging
    mov eax, cr0
    or eax, 31
    mov cr0, eax
    ;Cr0 protected mode enable
    mov eax, cr0
    or eax, 1
    mov cr0, eax

    ;Cr4 physical address extension
    mov eax, cr4
    or eax, 5
    mov cr4, eax

    ;efer long mode enable
    mov ecx, 0xC0000080               
    rdmsr    
 
    or eax, 0x00000100             
    wrmsr

    ;Load Cr3
    mov eax, [DataTrampoline.Paging]
    mov cr3, eax

    
    lgdt [DataTrampoline.GDTPointer]
 
    jmp 0x08:TrampolineLongMode
    hlt

[BITS 64]

TrampolineLongMode:
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    hlt





;------------------------------Data-----------------------------------
[BITS 64] 
ALIGN 64

GLOBAL DataTrampoline

DataTrampoline:
	.Status:			        db	0
    .GDTPointer:                dq  0
    .Paging:                    dq  0
