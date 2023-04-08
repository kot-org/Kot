GLOBAL _start
EXTERN main

exit:
	mov rdi, rax
	mov rax, 0x7
	syscall
	jmp iddle

_start:
	and rsp, 0xfffffffffffffff0 ; 16 byte aligned
	mov rbp, 0
	call main
	jmp exit

iddle:
	jmp iddle
