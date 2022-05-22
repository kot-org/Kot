[BITS 64]

%include "src/arch/x86-64/cpu/cpu.inc"

EXTERN  SyscallDispatch, InterruptHandler
GLOBAL	SyscallEnable

SyscallEnable:
	; Load segments into STAR MSR
	mov		rcx, 0xc0000081
	rdmsr
	
	mov 	edx, edi
	sal 	esi, 16
	or      edx, esi
	wrmsr
	
	; Load handler RIP into LSTAR MSR
	mov		rcx, 0xc0000082
	mov		rax, SyscallEntry
	mov		rdx, rax
	shr		rdx, 32
	wrmsr

	; Setup flags for syscall
	mov		rcx, 0xC0000084
	rdmsr
	or		eax, 0xfffffffe 
	wrmsr

	; Enable syscall / sysret instruction
	mov		rcx, 0xc0000080
	rdmsr
	or		rax, 1 ; enable syscall extension
	wrmsr

	ret

SyscallEntry: 
    swapgs

	mov [gs:0x10], rsp								        ; save userspace stack
	mov	rsp, [gs:0x8]                                       ; task syscall stack
	mov	rsp, [rsp + 0x0]  
	mov [rsp + 0x0], rbp
	mov rbp, [gs:0x8] 

	cld ; clear DF to push correctly to the stack

    push qword [rbp + 0x10]    	; ss
    push qword [gs:0x10]    	; rsp
    push r11                	; rflags
    push qword [rbp + 0x8]    	; cs
    push rcx                	; rip
    push 0x0                	; error code
    push 0x0                	; interrupt number
	mov rbp, [rsp + 0x38]

	sti
	int 0x40
	PUSH_REG
	mov rax, [gs:0x8] 
    mov rdi, rsp
    mov rsi, [rax + 0x18]


    call SyscallDispatch
	
	POP_REG
	cli
	mov	rsp, [rsp + 40] 
    add rsp, 56 

    swapgs	 
	o64 sysret

