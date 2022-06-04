.intel_syntax noprefix

.global _start
.extern main


.section .text

exit:
	mov rdi, [gs:0x0]
	mov rsi, rax
	mov rax, 0x7
	syscall

_start:
	call main
	jmp exit

.size _start, . - _start