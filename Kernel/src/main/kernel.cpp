#include <main/kernelInit.h>

extern "C" void main(stivale2_struct* stivale2_struct)
{   
    InitializeKernel(stivale2_struct);

    while(true){
        asm("hlt");
    };
}


