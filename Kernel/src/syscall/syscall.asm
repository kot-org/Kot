[BITS 64]

EXTERN  SyscallHandler, InterruptHandler
GLOBAL	syscallEnable

%macro    PUSH_REG    0
    push    r15
    push    r14
    push    r13
    push    r12
    push    r11
    push    r10
    push    r9
    push    r8
    push    rbp
    push    rdi
    push    rsi
    push    rdx
    push    rcx
    push    rbx
    push    rax
%endmacro
%macro    POP_REG        0
    pop    rax
    pop    rbx
    pop    rcx
    pop    rdx
    pop    rsi
    pop    rdi
    pop    rbp
    pop    r8
    pop    r9
    pop    r10
    pop    r11
    pop    r12
    pop    r13
    pop    r14
    pop    r15
%endmacro

syscallEnable:
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
	; Enable syscall / sysret instruction
	mov		rcx, 0xc0000080
	rdmsr
	or		rax, 1
	wrmsr

	ret

SyscallEntry: 
	cli
    swapgs

	mov [gs:0x10], rsp								        ; save userspace stack
	mov	rsp, [gs:0x8]                                       ; cpu stack

    push qword [gs:0x18]    ; ss
    push qword [gs:0x10]    ; rsp
    push r11                ; rflags
    push qword [gs:0x20]    ; cs
    push rcx                ; rip
    push 0x0                ; error code
    push 0x0                ; interrupt number

	PUSH_REG

    mov rdi, rsp
    mov rsi, [gs:0x0]

    call SyscallHandler

    cmp    rax, 0xff
    jnz    SoftReturn

    POP_REG
    add  rsp, 16 
    swapgs
	sti
    iretq

SoftReturn:
    POP_REG
    add  rsp, 56 
	mov	rsp, [gs:0x16] 
    swapgs
	sti
	o64 sysret

