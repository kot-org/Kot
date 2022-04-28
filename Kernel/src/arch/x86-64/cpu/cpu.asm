GLOBAL DisableInterrupts, EnableInterrupts, ReloadGSFS, GetCPUContext, GetAPICID, SetCPUContext, GetCPUContext, SetCPUGSBase, SetCPUGSKernelBase, SetCPUFSBase

%Define FS_Base             0xC0000100
%Define GS_Base             0xC0000101
%Define GS_KernelBase       0xC0000102

DisableInterrupts:
    cli
    ret

EnableInterrupts:
    sti
    ret

ReloadGSFS:
    mov     ax, 0x0
    mov     gs, ax
    mov     fs, ax
    ret

SetCPUGSBase:
	mov		eax, edi					
	shr		rdi, 32
	mov		edx, edi
    mov		ecx, GS_Base	
	wrmsr
    ret

SetCPUGSKernelBase:
	mov		eax, edi					
	shr		rdi, 32
	mov		edx, edi
    mov		ecx, GS_KernelBase	
	wrmsr
    ret

SetCPUFSBase:
	mov		eax, edi					
	shr		rdi, 32
	mov		edx, edi
    mov		ecx, FS_Base	
	wrmsr
    ret

GetCPUContext:
    mov rax, [gs:(rdi * 8)]
    ret

SetCPUContext:
    mov [gs:(rdi * 8)], rsi
    ret

GetAPICID:
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