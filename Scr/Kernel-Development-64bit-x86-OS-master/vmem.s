.data

.global load_cr3
load_cr3:
	movq %rdi, %rax
	movq %rax, %cr3
	retq
