.section .text

.global _start

exit:
	mov %rdi, %rax
	mov %rax, 0x7
	syscall
	jmp iddle

iddle:
	jmp iddle

_start:
	and %rsp, 0xfffffffffffffff0
	mov %rbp, 0
	
	mov $main, %rdi
	call __mlibc_entry
	jmp exit


.size exit, . - _start
.section .note.GNU-stack,"",%progbits

