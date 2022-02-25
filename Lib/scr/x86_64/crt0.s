.section .text

.global _start

_start:
	call main
	

.size _start, . - _start