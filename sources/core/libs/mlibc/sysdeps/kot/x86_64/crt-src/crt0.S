.section .text

.global _start
_start:
	mov $main, %rsi
    mov %rsp, %rdi
	call __mlibc_entry

.size _start, . - _start
.section .note.GNU-stack,"",%progbits

