#include <interrupts/interrupts.h>

void InitializeInterrupts(KernelInfo* kernelInfo){
    kernelInfo->IsIRQEventsFree = (bool*)calloc(kernelInfo->IRQSize * sizeof(bool));
    for(size64_t i = 0; i < kernelInfo->IRQSize; i++){
        if(kernelInfo->IRQEvents[i] != NULL){
            if(i < kernelInfo->IRQLineStart || i > (kernelInfo->IRQLineStart + kernelInfo->IRQLineSize)){
                kernelInfo->IsIRQEventsFree[i] = true;
            }
        }
    }
}