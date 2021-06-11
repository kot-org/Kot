#include "smp.h"

extern "C" void TrampolineEnd(){
    printf("test");
    globalGraphics->Update();
}

