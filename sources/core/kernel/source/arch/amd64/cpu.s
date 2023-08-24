global reload_gs_fs, get_cpu_context, cpu_get_apicid, set_cpu_context, get_cpu_context, set_cpu_gs_base, set_cpu_gs_kernel_base, set_cpu_fs_base

%define fs_base             0xc0000100
%define gs_base             0xc0000101
%define gs_kernel_base      0xc0000102

reload_gs_fs:
    mov     ax, 0x0
    mov     gs, ax
    mov     fs, ax
    ret

set_cpu_gs_base:
	mov		eax, edi					
	shr		rdi, 32
	mov		edx, edi
    mov		ecx, gs_base	
	wrmsr
    ret

set_cpu_gs_kernel_base:
	mov		eax, edi
	shr		rdi, 32
	mov		edx, edi
    mov		ecx, gs_kernel_base	
	wrmsr
    ret

set_cpu_fs_base:
	mov		eax, edi					
	shr		rdi, 32
	mov		edx, edi
    mov		ecx, fs_base	
	wrmsr
    ret

need_cpuid:
    mov    rax, 1
    cpuid
    shr    rbx, 24

    mov    rax, rbx
    ret

cpu_get_apicid:
    mov    ax, gs
    cmp    ax, 0x0
    jnz    need_cpuid
    mov    rax, [gs:0x0]
    ret 