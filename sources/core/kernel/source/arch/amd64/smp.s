[bits 16]

global trampoline_entry

%define code_seg     0x0008
%define data_seg     0x0010
%define target(addr) ((addr - trampoline_entry) + 0x1000)

;------------------------------code-----------------------------------

trampoline_entry: 
    cli
        
    mov	byte [target(data_trampoline.status)], 1
    
    ;cr4 physical address extension
    mov eax, cr4
    or eax, 1 << 5
    mov cr4, eax

    ;long mode enable efer
    mov ecx, 0xc0000080
    rdmsr
    or eax, 1 << 8
    wrmsr
    
    ;load cr3
    mov eax, [target(data_trampoline.paging)]
    mov cr3, eax

    ;cr0 protected mode enable and paging
    mov eax, cr0
    or eax, 0x80000001
    mov cr0, eax

    lgdt [target(gdt.pointer)]

    jmp code_seg:target(trampoline_long_mode)
    hlt

gdt:
.null:
    dq 0x0000000000000000
.code:
    dq 0x00AF9A000000FFFF  
.date:       
    dq 0x00CF92000000FFFF             
 
align 4
    dw 0                             
 
.pointer:
    dw target($ - gdt - 1)                 
    dd target(gdt)                            

[bits 64]

trampoline_long_mode:
    mov ax, data_seg
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    mov rsp, qword [data_trampoline.stack]

    mov	byte [data_trampoline.status], 3

    jmp [data_trampoline.main_entry]
    hlt


;------------------------------data-----------------------------------
[bits 64] 

global data_trampoline

data_trampoline:
	.status:			        db	0
    .paging:                    dq  0
    .stack:                     dq  0
    .stack_scheduler:           dq  0
    .main_entry:                dq  0    

times 4096 - ($ - $$) db 0