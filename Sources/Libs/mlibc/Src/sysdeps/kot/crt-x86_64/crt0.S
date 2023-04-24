.section .text

.global _start
_start:
	mov $main, %rsi
	call __mlibc_entry

.size _start, . - _start
.section .note.GNU-stack,"",%progbits
