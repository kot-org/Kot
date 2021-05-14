[bits 64]

GLOBAL GetRax, GetRcx, GetRdx, GetRbx, GetRsp, GetRbp, GetRsi, GetRdi, GetR8, GetR9, GetR10, GetR11, GetR12, GetR13, GetR14, GetR15
GLOBAL SetRax, SetRcx, SetRdx, SetRbx, SetRsp, SetRbp, SetRsi, SetRdi, SetR8, SetR9, SetR10, SetR11, SetR12, SetR13, SetR14, SetR15

GetRax:
    sub rsp, 0x80
    mov rax, [rsp-0x80]
    add rsp, 0x80
    ret

GetRcx:
    sub rsp, 0x80
    mov rax, [rsp-0x78]
    add rsp, 0x80
    ret

GetRdx:
    sub rsp, 0x80
    mov rax, [rsp-0x70]
    add rsp, 0x80
    ret

GetRbx:
    sub rsp, 0x80
    mov rax, [rsp-0x68]
    add rsp, 0x80

GetRsp:
    sub rsp, 0x80
    mov rax, [rsp-0x60]
    add rsp, 0x80
    ret

GetRbp:
    sub rsp, 0x80
    mov rax, [rsp-0x58]
    add rsp, 0x80
    ret

GetRsi:
    sub rsp, 0x80
    mov rax, [rsp-0x50]
    add rsp, 0x80
    ret

GetRdi:
    sub rsp, 0x80
    mov rax, [rsp-0x48]
    add rsp, 0x80
    ret

GetR8:
    sub rsp, 0x80
    mov rax, [rsp-0x40]
    add rsp, 0x80
    ret

GetR9:
    sub rsp, 0x80
    mov rax, [rsp-0x38]
    add rsp, 0x80
    ret

GetR10:
    sub rsp, 0x80
    mov rax, [rsp-0x30]
    add rsp, 0x80
    ret

GetR11:
    sub rsp, 0x80
    mov rax, [rsp-0x28]
    add rsp, 0x80
    ret

GetR12:
    sub rsp, 0x80
    mov rax, [rsp-0x20]
    add rsp, 0x80
    ret

GetR13:
    sub rsp, 0x80
    mov rax, [rsp-0x18]
    add rsp, 0x80
    ret

GetR14:
    sub rsp, 0x80
    mov rax, [rsp-0x10]
    add rsp, 0x80
    ret

GetR15:
    sub rsp, 0x80
    mov rax, [rsp-0x8]
    add rsp, 0x80
    ret

; Set registers 

SetRax:
    sub rsp, 0x80
    mov [rsp-0x80], rdi
    add rsp, 0x80

SetRcx:
    sub rsp, 0x80
    mov [rsp-0x78], rdi
    add rsp, 0x80

SetRdx:
    sub rsp, 0x80
    mov [rsp-0x70], rdi
    add rsp, 0x80

SetRbx:
    sub rsp, 0x80
    mov [rsp-0x68], rdi
    add rsp, 0x80

SetRsp:
    sub rsp, 0x80
    mov [rsp-0x60], rdi
    add rsp, 0x80

SetRbp:
    sub rsp, 0x80
    mov [rsp-0x58], rdi
    add rsp, 0x80

SetRsi:
    sub rsp, 0x80
    mov [rsp-0x50], rdi
    add rsp, 0x80

SetRdi:
    sub rsp, 0x80
    mov [rsp-0x48], rdi
    add rsp, 0x80

SetR8:
    sub rsp, 0x80
    mov [rsp-0x40], rdi
    add rsp, 0x80

SetR9:
    sub rsp, 0x80
    mov [rsp-0x38], rdi
    add rsp, 0x80

SetR10:
    sub rsp, 0x80
    mov [rsp-0x30], rdi
    add rsp, 0x80

SetR11:
    sub rsp, 0x80
    mov [rsp-0x28], rdi
    add rsp, 0x80

SetR12:
    sub rsp, 0x80
    mov [rsp-0x20], rdi
    add rsp, 0x80

SetR13:
    sub rsp, 0x80
    mov [rsp-0x18], rsi
    add rsp, 0x80

SetR14:
    sub rsp, 0x80
    mov [rsp-0x10], rsi
    add rsp, 0x80

SetR15:
    sub rsp, 0x80
    mov [rsp-0x8], rsi
    add rsp, 0x80
