#include <main/kernelInit.h>

extern "C" void main(BootInfo* bootInfo)
{   
    InitializeKernel(bootInfo);

    while(true){
        asm("hlt");
    };
}


