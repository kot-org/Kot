GLOBAL CreatCPUContext, GetCPUContext, GetCoreID, SetCPUContext

%Define GS_Base     0xC0000101
%Define GS_Kernel   0xC0000102

CreatCPUContext:
    mov     ax, 0x0
    mov     gs, ax
    mov     fs, ax
	mov		eax, edi					
	shr		rdi, 32
	mov		edx, edi
    mov		ecx, GS_Base	
	wrmsr
    ret

GetCPUContext:
    mov rax, [gs:(rdi * 8)]
    ret

SetCPUContext:
    mov [gs:(rdi * 8)], rsi
    ret

GetCoreID:
    mov    ax, gs
    cmp    ax, 0x0
    jnz    NeedCPUID
    mov    rax, [gs:0x0]
    ret 

NeedCPUID:
    mov    rax, 1
    cpuid
    shr    rbx, 24

    mov    rax, rbx
    ret