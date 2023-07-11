#pragma once
#include <lib/types.h>
#include <arch/x86-64/core.h>

enum CPUID_FEAT{
    CPUID_FEAT_ECX_SSE3         = 1 << 0, 
    CPUID_FEAT_ECX_PCLMUL       = 1 << 1,
    CPUID_FEAT_ECX_DTES64       = 1 << 2,
    CPUID_FEAT_ECX_MONITOR      = 1 << 3,  
    CPUID_FEAT_ECX_DS_CPL       = 1 << 4,  
    CPUID_FEAT_ECX_VMX          = 1 << 5,  
    CPUID_FEAT_ECX_SMX          = 1 << 6,  
    CPUID_FEAT_ECX_EST          = 1 << 7,  
    CPUID_FEAT_ECX_TM2          = 1 << 8,  
    CPUID_FEAT_ECX_SSSE3        = 1 << 9,  
    CPUID_FEAT_ECX_CID          = 1 << 10,
    CPUID_FEAT_ECX_SDBG         = 1 << 11,
    CPUID_FEAT_ECX_FMA          = 1 << 12,
    CPUID_FEAT_ECX_CX16         = 1 << 13, 
    CPUID_FEAT_ECX_XTPR         = 1 << 14, 
    CPUID_FEAT_ECX_PDCM         = 1 << 15, 
    CPUID_FEAT_ECX_PCID         = 1 << 17, 
    CPUID_FEAT_ECX_DCA          = 1 << 18, 
    CPUID_FEAT_ECX_SSE4_1       = 1 << 19, 
    CPUID_FEAT_ECX_SSE4_2       = 1 << 20, 
    CPUID_FEAT_ECX_X2APIC       = 1 << 21, 
    CPUID_FEAT_ECX_MOVBE        = 1 << 22, 
    CPUID_FEAT_ECX_POPCNT       = 1 << 23, 
    CPUID_FEAT_ECX_TSC          = 1 << 24, 
    CPUID_FEAT_ECX_AES          = 1 << 25, 
    CPUID_FEAT_ECX_XSAVE        = 1 << 26, 
    CPUID_FEAT_ECX_OSXSAVE      = 1 << 27, 
    CPUID_FEAT_ECX_AVX          = 1 << 28,
    CPUID_FEAT_ECX_F16C         = 1 << 29,
    CPUID_FEAT_ECX_RDRAND       = 1 << 30,
    CPUID_FEAT_ECX_HYPERVISOR   = 1 << 31,
 
    CPUID_FEAT_EDX_FPU          = 1 << 0,  
    CPUID_FEAT_EDX_VME          = 1 << 1,  
    CPUID_FEAT_EDX_DE           = 1 << 2,  
    CPUID_FEAT_EDX_PSE          = 1 << 3,  
    CPUID_FEAT_EDX_TSC          = 1 << 4,  
    CPUID_FEAT_EDX_MSR          = 1 << 5,  
    CPUID_FEAT_EDX_PAE          = 1 << 6,  
    CPUID_FEAT_EDX_MCE          = 1 << 7,  
    CPUID_FEAT_EDX_CX8          = 1 << 8,  
    CPUID_FEAT_EDX_APIC         = 1 << 9,  
    CPUID_FEAT_EDX_SEP          = 1 << 11, 
    CPUID_FEAT_EDX_MTRR         = 1 << 12, 
    CPUID_FEAT_EDX_PGE          = 1 << 13, 
    CPUID_FEAT_EDX_MCA          = 1 << 14, 
    CPUID_FEAT_EDX_CMOV         = 1 << 15, 
    CPUID_FEAT_EDX_PAT          = 1 << 16, 
    CPUID_FEAT_EDX_PSE36        = 1 << 17, 
    CPUID_FEAT_EDX_PSN          = 1 << 18, 
    CPUID_FEAT_EDX_CLFLUSH      = 1 << 19, 
    CPUID_FEAT_EDX_DS           = 1 << 21, 
    CPUID_FEAT_EDX_ACPI         = 1 << 22, 
    CPUID_FEAT_EDX_MMX          = 1 << 23, 
    CPUID_FEAT_EDX_FXSR         = 1 << 24, 
    CPUID_FEAT_EDX_SSE          = 1 << 25, 
    CPUID_FEAT_EDX_SSE2         = 1 << 26, 
    CPUID_FEAT_EDX_SS           = 1 << 27, 
    CPUID_FEAT_EDX_HTT          = 1 << 28, 
    CPUID_FEAT_EDX_TM           = 1 << 29, 
    CPUID_FEAT_EDX_IA64         = 1 << 30,
    CPUID_FEAT_EDX_PBE          = 1 << 31
};

enum CR0{
    CR0_PROTECTED_MODE_ENABLE       = 1 << 0,
    CR0_MONITOR_CO_PROCESSOR        = 1 << 1,
    CR0_EMULATION                   = 1 << 2,
    CR0_TASK_SWITCHED               = 1 << 3,
    CR0_EXTENSION_TYPE              = 1 << 4,
    CR0_NUMERIC_ERROR_ENABLE        = 1 << 5,
    CR0_WRITE_PROTECT_ENABLE        = 1 << 16,
    CR0_ALIGNMENT_MASK              = 1 << 18,
    CR0_NOT_WRITE_THROUGH_ENABLE    = 1 << 29,
    CR0_CACHE_DISABLE               = 1 << 30,
    CR0_PAGING_ENABLE               = 1 << 31,
};

enum CR4{
    CR4_VIRTUAL_8086_MODE_EXT               = 1 << 0,
    CR4_PROTECTED_MODE_VIRTUAL_INT          = 1 << 1,
    CR4_TIME_STAMP_DISABLE                  = 1 << 2, 
    CR4_DEBUGGING_EXT                       = 1 << 3,      
    CR4_PAGE_SIZE_EXT                       = 1 << 4,
    CR4_PHYSICAL_ADDRESS_EXT                = 1 << 5,
    CR4_MACHINE_CHECK_EXCEPTION             = 1 << 6,
    CR4_PAGE_GLOBAL_ENABLE                  = 1 << 7,
    CR4_PERFORMANCE_COUNTER_ENABLE          = 1 << 8,
    CR4_FXSR_ENABLE                         = 1 << 9,
    CR4_SIMD_EXCEPTION_SUPPORT              = 1 << 10,
    CR4_USER_MODE_INSTRUCTION_PREVENTION    = 1 << 11,
    CR4_5_LEVEL_PAGING_ENABLE               = 1 << 12,
    CR4_VIRTUAL_MACHINE_EXT_ENABLE          = 1 << 13,
    CR4_SAFER_MODE_EXT_ENABLE               = 1 << 14,
    CR4_FS_GS_BASE_ENABLE                   = 1 << 16,
    CR4_PCID_ENABLE                         = 1 << 17,
    CR4_XSAVE_ENABLE                        = 1 << 18,
    CR4_SUPERVISOR_EXE_PROTECTION_ENABLE    = 1 << 20,
    CR4_SUPERVISOR_ACCESS_PROTECTION_ENABLE = 1 << 21,
    CR4_KEY_PROTECTION_ENABLE               = 1 << 22,
    CR4_CONTROL_FLOW_ENABLE                 = 1 << 23,
    CR4_SUPERVISOR_KEY_PROTECTION_ENABLE    = 1 << 24,
};

enum XCR0{
    XCR0_XSAVE_SAVE_X87     = 1 << 0,
    XCR0_XSAVE_SAVE_SSE     = 1 << 1,
    XCR0_AVX_ENABLE         = 1 << 2,
    XCR0_BNDREG_ENABLE      = 1 << 3,
    XCR0_BNDCSR_ENABLE      = 1 << 4,
    XCR0_AVX512_ENABLE      = 1 << 5,
    XCR0_ZMM0_15_ENABLE     = 1 << 6,
    XCR0_ZMM16_32_ENABLE    = 1 << 7,
    XCR0_PKRU_ENABLE        = 1 << 9,
};

struct CPURegisters{
    uint64_t rax;
    uint64_t rbx;
    uint64_t rcx;
    uint64_t rdx;
    uint64_t rsi;
    uint64_t rdi;
    uint64_t rsp;
    uint64_t rbp;
    uint64_t r8;
    uint64_t r9;
    uint64_t r10;
    uint64_t r11;
    uint64_t r12;
    uint64_t r13;
    uint64_t r14;
    uint64_t r15;
};

struct cr4_t{
    bool VME:1;
    bool PVI:1;
    bool TSD:1;
    bool DE:1;
    bool PSE:1;
    bool PAE:1;
    bool MCE:1;
    bool PGE:1;
    bool PCE:1;
    bool OSFXSR:1;
    bool OSXMMEXCPT:1;
    bool UMIP:1;
    bool Reserved0:1;
    bool VMXE:1;
    bool SMXE:1;
    bool Reserved1:1;
    bool FSGSBASE:1;
    bool PCIDE:1;
    bool OSXSAVE:1;
    bool Reserved2:1;
    bool SMEP:1;
    bool SMAP:1;
    bool PKE:1;
    bool CET:1;
    bool PKS:1;
}__attribute__((packed));

struct cr0_t{
    bool PE:1;
    bool MP:1;
    bool EM:1;
    bool TS:1;
    bool ET:1;
    bool Reserved0:1;
    bool Reserved1:1;
    bool Reserved2:1;
    bool Reserved3:1;
    bool Reserved4:1;
    bool Reserved5:1;
    bool Reserved6:1;
    bool Reserved7:1;
    bool Reserved8:1;
    bool Reserved9:1;
    bool Reserved10:1;
    bool WP:1;
    bool Reserved11:1;
    bool AM:1;
    bool Reserved12:1;
    bool Reserved13:1;
    bool Reserved14:1;
    bool Reserved15:1;
    bool Reserved16:1;
    bool Reserved17:1;
    bool Reserved18:1;
    bool Reserved19:1;
    bool Reserved20:1;
    bool Reserved21:1;
    bool NW:1;
    bool CD:1;
    bool PG:1;
}__attribute__((packed));

struct xcr0_t{
    bool X87:1; 
    bool SSE:1; 
    bool AVX:1; 
    bool BNDREG:1; 
    bool BNDCSR:1; 
    bool OPMASK:1; 
    bool ZMM_Hi256:1; 
    bool Hi16_ZMM:1; 
    bool Reserved0:1; 
    bool PKRU:1; 
}__attribute__((packed));

struct rflags_t{
    bool CF:1;
    bool Reserved0:1; //this is bit is set as default
    bool PF:1;
    bool Reserved1:1;
    bool AF:1;
    bool Reserved2:1;
    bool ZF:1;
    bool SF:1;
    bool TF:1;
    bool IF:1;
    bool DF:1;
    bool OF:1;
    uint8_t IOPL:2;
    bool NT:1;
    bool RF:1;
    bool VM:1;
    bool AC:1;
    bool VIF:1;
    bool VIP:1;
    bool ID:1;
    bool Reserved3:1;
    bool Reserved4:1;
    bool Reserved5:1;
    bool Reserved6:1;
    bool Reserved7:1;
    bool Reserved8:1;
    bool Reserved9:1;
    bool Reserved10:1;
    bool Reserved11:1;
    bool Reserved12:1;
    bool Reserved13:1;
    uint32_t Reserved14;
}__attribute__((packed));


struct ContextStack {
    uint64_t cr3;

    struct threadInfo_t* threadInfo; /* gs:0x8 */
    
    uint64_t GlobalPurpose; /* rax use for return and data */
    uint64_t rbx;       /* rbx */
    uint64_t arg3;      /* rcx */
    uint64_t arg2;      /* rdx */ 
    uint64_t arg1;      /* rsi */  
    uint64_t arg0;      /* rdi */ 
    uint64_t rbp;       /* rbp */

    uint64_t arg4;      /* r8  */ 
    uint64_t arg5;      /* r9  */ 
    uint64_t r10;       /* r10 */ 
    uint64_t r11;       /* r11 */
    uint64_t r12;       /* r12 */
    uint64_t r13;       /* r13 */
    uint64_t r14;       /* r14 */
    uint64_t r15;       /* r15 */

    uint64_t InterruptNumber; 
    uint64_t ErrorCode; 
    
    uint64_t rip; 
    uint64_t cs; 
    rflags_t rflags; 
    uint64_t rsp; 
    uint64_t ss;
}__attribute__((packed)); 

struct SyscallStack {
    uint64_t cr3;

    struct threadInfo_t* threadInfo; /* gs:0x8 */
    
    uint64_t GlobalPurpose; /* rax use for return and data */
    uint64_t rbx;       /* rbx */
    uint64_t rcx;       /* rcx */
    uint64_t arg2;      /* rdx */ 
    uint64_t arg1;      /* rsi */  
    uint64_t arg0;      /* rdi */ 
    uint64_t rbp;       /* rbp */

    uint64_t arg5;      /* r8  */ 
    uint64_t arg4;      /* r9  */ 
    uint64_t arg3;      /* r10 */ 
    uint64_t r11;       /* r11 */
    uint64_t r12;       /* r12 */
    uint64_t r13;       /* r13 */
    uint64_t r14;       /* r14 */
    uint64_t r15;       /* r15 */

    uint64_t InterruptNumber; 
    uint64_t ErrorCode; 
    
    uint64_t rip; 
    uint64_t cs; 
    rflags_t rflags; 
    uint64_t rsp; 
    uint64_t ss;
}__attribute__((packed)); 

struct CPUContext{
    uint64_t ID;
    uint64_t SyscallStack;
    uint64_t UserStack;
    uint32_t FeaturesECX;
    uint32_t FeaturesEDX;
}__attribute__((packed));

enum CPUContextIndex{
    CPUContextIndex_ID              = 0,
    CPUContextIndex_SyscallStack    = 1,
    CPUContextIndex_UserStack       = 2,
    CPUContextIndex_FeaturesECX     = 3,
    CPUContextIndex_FeaturesEDX     = 4,
};

namespace CPU{
    void InitCPU(struct ArchInfo_t* ArchInfo);
    void InitCore();
    extern "C" void DisableInterrupts();
    extern "C" void EnableInterrupts();
    extern "C" void ReloadGSFS();
    extern "C" void SetCPUGSBase(uint64_t Value);
    extern "C" void SetCPUGSKernelBase(uint64_t Value);
    extern "C" void SetCPUFSBase(uint64_t Value);
    extern "C" uint64_t GetCPUContext(uint64_t index);
    extern "C" void SetCPUContext(uint64_t index, uint64_t value);
    extern "C" uint8_t GetAPICID();
    void cpuid(uint32_t reg, uint32_t *eax, uint32_t *ebx, uint32_t *ecx, uint32_t *edx);
    void GetMSR(uint32_t index, uint32_t* low, uint32_t* high);
    void SetMSR(uint32_t index, uint32_t low, uint32_t high);
    void GetFeatures(uint32_t FeaturesECX, uint32_t FeaturesEDX);
    uint8_t GetCodeRing(ContextStack* Registers);
};
