[bits 64]

GLOBAL Entry_DivideByZero_Handler, Entry_Debug_Handler, Entry_NMI_Handler, Entry_Breakpoint_Handler, Entry_Overflow_Handler, Entry_BoundRangeExceeded_Handler, Entry_InvalidOpcode_Handler, Entry_DeviceNotAvailable_Handler, Entry_DoubleFault_Handler, Entry_InvalidTSS_Handler, Entry_SegmentNotPresent_Handler, Entry_StackSegmentFault_Handler, Entry_GPFault_Handler, Entry_PageFault_Handler, Entry_x87FloatingPointException_Handler, Entry_AlignmentCheck_Handler, Entry_MachineCheck_Handler, Entry_SIMDFloatingPointException_Handler, Entry_VirtualizationException_Handler, Entry_SecurityException_Handler, Entry_LAPICTIMERInt_Handler, Entry_SyscallInt_Handler, Entry_IRQ0_Handler, Entry_IRQ1_Handler, Entry_IRQ2_Handler, Entry_IRQ3_Handler, Entry_IRQ4_Handler, Entry_IRQ5_Handler, Entry_IRQ6_Handler, Entry_IRQ7_Handler, Entry_IRQ8_Handler, Entry_IRQ9_Handler, Entry_IRQ10_Handler, Entry_IRQ11_Handler, Entry_IRQ12_Handler, Entry_IRQ13_Handler, Entry_IRQ14_Handler, Entry_IRQ15_Handler, Entry_IRQ16_Handler
EXTERN DivideByZero_Handler, Debug_Handler, NMI_Handler, Breakpoint_Handler, Overflow_Handler, BoundRangeExceeded_Handler, InvalidOpcode_Handler, DeviceNotAvailable_Handler, DoubleFault_Handler, InvalidTSS_Handler, SegmentNotPresent_Handler, StackSegmentFault_Handler, GPFault_Handler, PageFault_Handler, x87FloatingPointException_Handler, AlignmentCheck_Handler, MachineCheck_Handler, SIMDFloatingPointException_Handler, VirtualizationException_Handler, SecurityException_Handler, LAPICTIMERInt_Handler, SyscallInt_Handler, IRQ0_Handler, IRQ1_Handler, IRQ2_Handler, IRQ3_Handler, IRQ4_Handler, IRQ5_Handler, IRQ6_Handler, IRQ7_Handler, IRQ8_Handler, IRQ9_Handler, IRQ10_Handler, IRQ11_Handler, IRQ12_Handler, IRQ13_Handler, IRQ14_Handler, IRQ15_Handler, IRQ16_Handler

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


Entry_DivideByZero_Handler:
    swapgs
    PUSH_REG

    mov    rax, 1
    cpuid
    shr    rbx, 24
    mov    rsi, rbx

    mov rdi, rsp

    call DivideByZero_Handler

    POP_REG
    swapgs
    
    iretq

Entry_Debug_Handler:
    swapgs
    PUSH_REG

    mov    rax, 1
    cpuid
    shr    rbx, 24
    mov    rsi, rbx

    mov rdi, rsp

    call Debug_Handler

    POP_REG
    swapgs
    
    iretq

Entry_NMI_Handler:
    swapgs
    PUSH_REG

    mov    rax, 1
    cpuid
    shr    rbx, 24
    mov    rsi, rbx

    mov rdi, rsp

    call NMI_Handler

    POP_REG
    swapgs
    
    iretq

Entry_Breakpoint_Handler:
    swapgs
    PUSH_REG

    mov    rax, 1
    cpuid
    shr    rbx, 24
    mov    rsi, rbx

    mov rdi, rsp

    call Breakpoint_Handler

    POP_REG
    swapgs
    
    iretq

Entry_Overflow_Handler:
    swapgs
    PUSH_REG

    mov    rax, 1
    cpuid
    shr    rbx, 24
    mov    rsi, rbx

    mov rdi, rsp

    call Overflow_Handler

    POP_REG
    swapgs
    
    iretq

Entry_BoundRangeExceeded_Handler:
    swapgs
    PUSH_REG

    mov    rax, 1
    cpuid
    shr    rbx, 24
    mov    rsi, rbx

    mov rdi, rsp

    call BoundRangeExceeded_Handler

    POP_REG
    swapgs
    
    iretq

Entry_InvalidOpcode_Handler:
    swapgs
    PUSH_REG

    mov    rax, 1
    cpuid
    shr    rbx, 24
    mov    rsi, rbx

    mov rdi, rsp

    call InvalidOpcode_Handler

    POP_REG
    swapgs
    
    iretq

Entry_DeviceNotAvailable_Handler:
    swapgs
    PUSH_REG

    mov    rax, 1
    cpuid
    shr    rbx, 24
    mov    rsi, rbx

    mov rdi, rsp

    call DeviceNotAvailable_Handler

    POP_REG
    swapgs
    
    iretq

Entry_DoubleFault_Handler:
    swapgs
    PUSH_REG

    mov    rax, 1
    cpuid
    shr    rbx, 24
    mov    rsi, rbx

    mov rdi, rsp

    call DoubleFault_Handler

    POP_REG
    swapgs
    
    iretq

Entry_InvalidTSS_Handler:
    swapgs
    PUSH_REG

    mov    rax, 1
    cpuid
    shr    rbx, 24
    mov    rsi, rbx

    mov rdi, rsp

    call InvalidTSS_Handler

    POP_REG
    swapgs
    
    iretq

Entry_SegmentNotPresent_Handler:
    swapgs
    PUSH_REG

    mov    rax, 1
    cpuid
    shr    rbx, 24
    mov    rsi, rbx

    mov rdi, rsp

    call SegmentNotPresent_Handler

    POP_REG
    swapgs
    
    iretq



Entry_StackSegmentFault_Handler:
    swapgs
    PUSH_REG

    mov    rax, 1
    cpuid
    shr    rbx, 24
    mov    rsi, rbx

    mov rdi, rsp

    call StackSegmentFault_Handler

    POP_REG
    swapgs
    
    iretq

Entry_GPFault_Handler:
    swapgs
    PUSH_REG

    mov    rax, 1
    cpuid
    shr    rbx, 24
    mov    rsi, rbx

    mov rdi, rsp

    call GPFault_Handler

    POP_REG
    swapgs

    iretq

Entry_PageFault_Handler:
    swapgs
    PUSH_REG

    mov    rax, 1
    cpuid
    shr    rbx, 24

    mov rsi, rbx
    mov rdi, rsp
    mov rdx, cr2

    call PageFault_Handler
    
    POP_REG
    swapgs
 
    iretq

Entry_x87FloatingPointException_Handler:
    swapgs
    PUSH_REG

    mov    rax, 1
    cpuid
    shr    rbx, 24
    mov    rsi, rbx

    mov rdi, rsp

    call x87FloatingPointException_Handler
    
    POP_REG
    swapgs
 
    iretq

Entry_AlignmentCheck_Handler:
    swapgs
    PUSH_REG

    mov    rax, 1
    cpuid
    shr    rbx, 24
    mov    rsi, rbx

    mov rdi, rsp

    call AlignmentCheck_Handler
    
    POP_REG
    swapgs
 
    iretq

Entry_MachineCheck_Handler:
    swapgs
    PUSH_REG

    mov    rax, 1
    cpuid
    shr    rbx, 24
    mov    rsi, rbx

    mov rdi, rsp

    call MachineCheck_Handler
    
    POP_REG
    swapgs
 
    iretq

Entry_SIMDFloatingPointException_Handler:
    swapgs
    PUSH_REG

    mov    rax, 1
    cpuid
    shr    rbx, 24
    mov    rsi, rbx

    mov rdi, rsp

    call SIMDFloatingPointException_Handler
    
    POP_REG
    swapgs
 
    iretq

Entry_VirtualizationException_Handler:
    swapgs
    PUSH_REG

    mov    rax, 1
    cpuid
    shr    rbx, 24
    mov    rsi, rbx

    mov rdi, rsp

    call VirtualizationException_Handler
    
    POP_REG
    swapgs
 
    iretq

Entry_SecurityException_Handler:
    swapgs
    PUSH_REG

    mov    rax, 1
    cpuid
    shr    rbx, 24
    mov    rsi, rbx

    mov rdi, rsp

    call SecurityException_Handler
    
    POP_REG
    swapgs
 
    iretq

Entry_LAPICTIMERInt_Handler:
    PUSH_REG    

    mov rax, 1
    cpuid
    shr rbx, 24
    mov rsi, rbx

    mov rdi, rsp
    
    call LAPICTIMERInt_Handler

    POP_REG  
	iretq 

Entry_SyscallInt_Handler:
    PUSH_REG    

    mov    rax, 1
    cpuid
    shr    rbx, 24
    mov    rsi, rbx

    mov rdi, rsp

    call SyscallInt_Handler

    POP_REG     
	iretq 

# IRQs

Entry_IRQ0_Handler:
    PUSH_REG
    mov rdi, rsp
    
    call IRQ0_Handler

    POP_REG  
	iretq 

Entry_IRQ1_Handler:
    PUSH_REG
    mov rdi, rsp
    
    call IRQ1_Handler

    POP_REG  
	iretq 

Entry_IRQ2_Handler:
    PUSH_REG
    mov rdi, rsp
    
    call IRQ2_Handler

    POP_REG  
	iretq 

Entry_IRQ3_Handler:
    PUSH_REG
    mov rdi, rsp
    
    call IRQ3_Handler

    POP_REG  
	iretq 

Entry_IRQ4_Handler:
    PUSH_REG
    mov rdi, rsp
    
    call IRQ4_Handler

    POP_REG  
	iretq 

Entry_IRQ5_Handler:
    PUSH_REG
    mov rdi, rsp
    
    call IRQ5_Handler

    POP_REG  
	iretq 

Entry_IRQ6_Handler:
    PUSH_REG
    mov rdi, rsp
    
    call IRQ6_Handler

    POP_REG  
	iretq 

Entry_IRQ7_Handler:
    PUSH_REG
    mov rdi, rsp
    
    call IRQ7_Handler

    POP_REG  
	iretq 

Entry_IRQ8_Handler:
    PUSH_REG
    mov rdi, rsp
    
    call IRQ8_Handler

    POP_REG  
	iretq 

Entry_IRQ9_Handler:
    PUSH_REG
    mov rdi, rsp
    
    call IRQ9_Handler

    POP_REG  
	iretq 

Entry_IRQ10_Handler:
    PUSH_REG
    mov rdi, rsp
    
    call IRQ10_Handler

    POP_REG  
	iretq 

Entry_IRQ11_Handler:
    PUSH_REG
    mov rdi, rsp
    
    call IRQ11_Handler

    POP_REG  
	iretq 

Entry_IRQ12_Handler:
    PUSH_REG
    mov rdi, rsp
    
    call IRQ12_Handler

    POP_REG  
	iretq 

Entry_IRQ13_Handler:
    PUSH_REG
    mov rdi, rsp
    
    call IRQ0_Handler

    POP_REG  
	iretq 

Entry_IRQ14_Handler:
    PUSH_REG
    mov rdi, rsp
    
    call IRQ0_Handler

    POP_REG  
	iretq 

Entry_IRQ15_Handler:
    PUSH_REG
    mov rdi, rsp
    
    call IRQ0_Handler

    POP_REG  
	iretq 

Entry_IRQ16_Handler:
    PUSH_REG
    mov rdi, rsp
    
    call IRQ0_Handler

    POP_REG  
	iretq 