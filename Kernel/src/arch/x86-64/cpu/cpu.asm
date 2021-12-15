GLOBAL SaveCoreID, GetCoreID

SaveCoreID:
    swapgs
    mov    rax, 1
    cpuid
    shr    rbx, 24
    mov    gs, rbx
    swapgs
    ret

GetCoreID:
    mov rax, gs
    ret