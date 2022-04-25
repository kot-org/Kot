.intel_syntax noprefix

.global _start
.extern main


.section .text

exit:


_start:
	call main
	jmp exit

.size _start, . - _start