GLOBAL _start
EXTERN main

exit:
	mov rdi, rax
	mov rax, 0x7
	syscall
	jmp iddle

_start:
	push 0
	mov rbp, rsp
	call main
	jmp exit

iddle:
	jmp iddle
