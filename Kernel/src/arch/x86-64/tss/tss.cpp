#include <arch/x86-64/tss/tss.h>

static TSS TSSdescriptors[MAX_PROCESSORS];

void TSSInit(){
    memset(TSSdescriptors, 0, sizeof(TSS) * MAX_PROCESSORS);
}

uint16_t TSSInstall(uint8_t numCPU){
    uint64_t tss_base = (uint64_t)&TSSdescriptors[numCPU];
    memset((void*)tss_base, 0, sizeof(TSS));

    uint16_t location = gdtInstallTSS(tss_base, sizeof(TSS));
    TSSdescriptors[numCPU].IOPBOffset = sizeof(TSS);
    return location;
}

void TSSSetStack(uint8_t numCPU, void* stack){
    CPU::SetCPUContext(CPUContextIndex_KernelStack, (uint64_t)stack);
    TSSdescriptors[numCPU].RSP[0] = (uint64_t)stack;
}

uint64_t TSSGetStack(uint8_t numCPU){
    return TSSdescriptors[numCPU].RSP[0];
}

TSS* TSSGet(uint8_t numCPU){
    return &TSSdescriptors[numCPU];
}