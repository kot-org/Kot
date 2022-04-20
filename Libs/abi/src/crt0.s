.intel_syntax noprefix

.global _start, _main_thread, _process
.extern main

.section .data
	_main_thread: .long 0
	_process: .long 0

.section .text
_start:
	call main
	mov rax, [gs:0x0]
	mov rbx, [rax + 0x0]
	mov [_main_thread], rbx
	mov rbx, [rax + 0x8]
	mov [_process], rbx
	

.size _start, . - _start