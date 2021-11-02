[bits 64]
GLOBAL LoadPaging

LoadPaging:
    mov cr3, rdi
    add rsp, rsi
    add rbp, rsi ; because gcc save rsp into rbp
    ret