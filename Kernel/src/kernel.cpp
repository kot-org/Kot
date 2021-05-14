#include "kernelInit.h"

void main(BootInfo* bootInfo)
{   
    KernelInfo kernelInfo = InitializeKernel(bootInfo);
    PageTableManager* pageTableManager = kernelInfo.pageTableManager;

    globalGraphics->Print("Kernel initialized successfully");
    globalGraphics->Next();
    
    while(true){
        globalGraphics->Update();   
    }

    while(true);
}


