GLOBAL SaveCoreID, GetCoreID

%Define GS_Kernel 0xC0000102

;TODO remove swap and replace by wmsr/rmsr

SaveCoreID:
    mov    rax, 1
    cpuid
    shr    rbx, 24

    mov    eax, ebx
    mov    edx, 0
    mov    ecx, GS_Kernel
    wrmsr
    ret

GetCoreID:
    mov    ecx, GS_Kernel
    rdmsr
    ret