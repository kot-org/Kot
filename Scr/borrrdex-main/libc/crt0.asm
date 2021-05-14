[bits 64]
GLOBAL _start
extern main
extern exit

_start:
    ; Setup end of stack frame linked list
    mov rbp, qword 0
    push rbp
    mov rbp, rsp

    call main

    mov edi, eax
    leave
    jmp exit
    