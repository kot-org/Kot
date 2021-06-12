#include "smp.h"

extern "C" void TrampolineMain(){
    //printf("test");
    //globalGraphics->Update();
    while(true){
        asm("hlt");
    }
}

