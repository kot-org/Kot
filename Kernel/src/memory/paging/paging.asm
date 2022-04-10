[bits 64]
GLOBAL LoadPaging

LoadPaging:
    mov cr3, rdi
    ret