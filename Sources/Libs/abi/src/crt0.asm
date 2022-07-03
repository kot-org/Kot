GLOBAL _start
EXTERN main

exit:
	mov rdi, [gs:0x0]
	mov rsi, rax
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