.intel_syntax noprefix

.global _start, self, parent

.section .data
	self: .long 0
	parent: .long 0

.section .text
_start:
	mov rax, [gs:0x0]
	mov rbx, [rax + 0x0]
	mov [self], rbx
	mov rbx, [rax + 0x0]
	mov [self], rbx
	call main
	

.size _start, . - _start