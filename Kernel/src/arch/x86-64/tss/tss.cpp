#include <arch/x86-64/tss/tss.h>

static TSS TSSdescriptors[MAX_PROCESSORS];

void TSSInit(){
    memset(TSSdescriptors, 0, sizeof(TSS) * MAX_PROCESSORS);
}

uint16_t TSSInstall(uint8_t numCPU){
    uint64_t tss_base = (uint64_t)&TSSdescriptors[numCPU];
    memset((void*)tss_base, 0, sizeof(TSS));

    uint16_t location = gdtInstallTSS(tss_base, sizeof(TSS));
    TSSdescriptors[numCPU].IOPBOffset = 0;

    // enable ist
    return location;
}

void TSSSetStack(uint8_t numCPU, void* stack){
    TSSdescriptors[numCPU].RSP[0] = (uint64_t)stack;
}

uint64_t TSSGetStack(uint8_t numCPU){
    return TSSdescriptors[numCPU].RSP[0];
}

void TSSSetIST(uint8_t numCPU, uint8_t position, uint64_t value){
    /* remov eposition to one because the 0 ist don't exist in the tss array */
    TSSdescriptors[numCPU].IST[position - 1] = value;
}

uint64_t TSSGetIST(uint8_t numCPU, uint8_t position){
    return TSSdescriptors[numCPU].IST[position - 1];
}

TSS* TSSGet(uint8_t numCPU){
    return &TSSdescriptors[numCPU];
}
