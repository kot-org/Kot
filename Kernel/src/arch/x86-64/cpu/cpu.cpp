#include "cpu.h"

CPU globalCPU; 


CPURegisters DumpRegisters(){
    CPURegisters regs;
    register uint64_t rax asm("rax");
    register uint64_t rbx asm("rbx");
    register uint64_t rcx asm("rcx");
    register uint64_t rdx asm("rdx");
    register uint64_t rsi asm("rsi");
    register uint64_t rsp asm("rsp");
    register uint64_t rbp asm("rbp");
    register uint64_t r8 asm("r8");
    register uint64_t r9 asm("r9");
    register uint64_t r10 asm("r10");
    register uint64_t r11 asm("r11");
    register uint64_t r12 asm("r12");
    register uint64_t r13 asm("r13");
    register uint64_t r14 asm("r14");
    register uint64_t r15 asm("r15");
    regs = {rax,rbx,rcx,rdx,rsi,rsp,rbp,r8,r9,r10,r11,r12,r13,r14,r15};
    return regs;
}

inline void CPU::cpuid(uint32_t reg, uint32_t *eax, uint32_t *ebx, uint32_t *ecx, uint32_t *edx)
{
    __asm__ volatile("cpuid"
        : "=a" (*eax), "=b" (*ebx), "=c" (*ecx), "=d" (*edx)
        : "0" (reg));
}

char vendor[128];
char* CPU::getName() {
    cpuid(0x80000002, (uint32_t *)(vendor +  0), (uint32_t *)(vendor +  4), (uint32_t *)(vendor +  8), (uint32_t *)(vendor + 12));
    cpuid(0x80000003, (uint32_t *)(vendor + 16), (uint32_t *)(vendor + 20), (uint32_t *)(vendor + 24), (uint32_t *)(vendor + 28));
    cpuid(0x80000004, (uint32_t *)(vendor + 32), (uint32_t *)(vendor + 36), (uint32_t *)(vendor + 40), (uint32_t *)(vendor + 44));
    vendor[127] = 0;
    return vendor;
}

char vendor2[13];
char* CPU::getVendorID(){
    uint32_t standardfunc;
    cpuid(0, &standardfunc, (uint32_t *)(vendor2 + 0), (uint32_t *)(vendor2 + 8), (uint32_t *)(vendor2 + 4));
    vendor[12] = 0;
    return vendor2;
}

void CPU::addSupported(const char* sup) {
    features[cpuFeatures] = (char*)sup;
    cpuFeatures++;
}

char** CPU::getFeatures() {
    uint32_t eax, ebx, ecx, edx, standardfunc;
    char* tmp[13];
    cpuid(0, &standardfunc, (uint32_t *)(tmp + 0), (uint32_t *)(tmp + 8), (uint32_t *)(tmp + 4));
    
    if(standardfunc >= 1) {
        cpuid(1,&eax,&ebx,&ecx,&edx);

        if(ecx & ECX_SSE3) addSupported("SSE3");
        if(ecx & ECX_PCLMULQDQ) addSupported("PCLMULQDQ");
        if(ecx & ECX_DTES64) addSupported("DTES64");
        if(ecx & ECX_MONITOR) addSupported("MONITOR");
        if(ecx & ECX_DS_CPL) addSupported("DS_CPL");
        if(ecx & ECX_VMX) addSupported("VMX");
        if(ecx & ECX_SMX) addSupported("SMX");
        if(ecx & ECX_EST) addSupported("EST");
        if(ecx & ECX_TM2) addSupported("TM2");
        if(ecx & ECX_SSSE3) addSupported("SSSE3");
        if(ecx & ECX_CNXT_ID) addSupported("CNXT_ID");
        if(ecx & ECX_FMA) addSupported("FMA");
        if(ecx & ECX_CX16) addSupported("CX16");
        if(ecx & ECX_XTPR) addSupported("XTPR");
        if(ecx & ECX_PDCM) addSupported("PDCM");
        if(ecx & ECX_PCID) addSupported("PCID");
        if(ecx & ECX_DCA) addSupported("DCA");
        if(ecx & ECX_SSE41) addSupported("SSE41");
        if(ecx & ECX_SSE42) addSupported("SSE42");
        if(ecx & ECX_X2APIC) addSupported("X2APIC");
        if(ecx & ECX_MOVBE) addSupported("MOVBE");
        if(ecx & ECX_POPCNT) addSupported("POPCNT");
        if(ecx & ECX_TSC) addSupported("TSC");
        if(ecx & ECX_AESNI) addSupported("AESNI");
        if(ecx & ECX_XSAVE) addSupported("XSAVE");
        if(ecx & ECX_OSXSAVE) addSupported("OSXSAVE");
        if(ecx & ECX_AVX) addSupported("AVX");
        if(ecx & ECX_F16C) addSupported("F16C");
        if(ecx & ECX_RDRAND) addSupported("RDRAND");
        if(edx & EDX_FPU) addSupported("FPU");
        if(edx & EDX_VME) addSupported("VME");
        if(edx & EDX_DE) addSupported("DE");
        if(edx & EDX_PSE) addSupported("PSE");
        if(edx & EDX_TSC) addSupported("TSC");
        if(edx & EDX_MSR) addSupported("MSR");
        if(edx & EDX_PAE) addSupported("PAE");
        if(edx & EDX_MCE) addSupported("MCE");
        if(edx & EDX_CX8) addSupported("CX8");
        if(edx & EDX_APIC) addSupported("APIC");
        if(edx & EDX_SEP) addSupported("SEP");
        if(edx & EDX_MTRR) addSupported("MTRR");
        if(edx & EDX_PGE) addSupported("PGE");
        if(edx & EDX_MCA) addSupported("MCA");
        if(edx & EDX_CMOV) addSupported("CMOV");
        if(edx & EDX_PAT) addSupported("PAT");
        if(edx & EDX_PSE36) addSupported("PSE36");
        if(edx & EDX_PSN) addSupported("PSN");
        if(edx & EDX_CLFLUSH) addSupported("CLFLUSH");
        if(edx & EDX_DS) addSupported("DS");
        if(edx & EDX_ACPI) addSupported("ACPI");
        if(edx & EDX_MMX) addSupported("MMX");
        if(edx & EDX_FXSR) addSupported("FXSR");
        if(edx & EDX_SSE) addSupported("SSE");
        if(edx & EDX_SSE2) addSupported("SSE2");
        if(edx & EDX_SS) addSupported("SS");
        if(edx & EDX_HTT) addSupported("HTT");
        if(edx & EDX_TM) addSupported("TM");
        if(edx & EDX_PBE) addSupported("PBE");
    } 
    
    return features;
}
