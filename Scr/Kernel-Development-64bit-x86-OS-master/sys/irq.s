.text

.global handler_timer
.global handler_keyboard
.global handler_syscall
.global handler_page_fault

//.extern keyboard_handler

handler_timer:
	
	callq timer_handler
	iretq
handler_keyboard:
       
	callq keyboard_handler
        iretq

handler_page_fault:
	pushq %rdi
        pushq %rax
        pushq %rbx
        pushq %rcx
        pushq %rdx
        pushq %rbp
        pushq %rsi
        pushq %r8
        pushq %r9
        movq %rsp,%rdi
        callq page_fault_handler
        popq %r9
        popq %r8
        popq %rsi
        popq %rbp
        popq %rdx
        popq %rcx
        popq %rbx
        popq %rax
        popq %rdi
        addq $8, %rsp
        iretq
	
handler_syscall:
	pushq %rdi
	pushq %rax
	pushq %rbx
	pushq %rcx
	pushq %rdx
	pushq %rbp
	pushq %rsi
	pushq %r8
	pushq %r9
	movq %rsp,%rdi
	callq register_interrupt_handler
	popq %r9
	popq %r8
	popq %rsi
	popq %rbp
	popq %rdx
	popq %rcx
	popq %rbx
	popq %rax
	popq %rdi
	iretq
