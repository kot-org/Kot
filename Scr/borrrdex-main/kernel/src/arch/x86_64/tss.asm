[bits 64]

GLOBAL __load_tss

__load_tss:
    push rbp
    mov rbp, rsp

    ltr di
    
    leave
    ret