#include <misc/panic/panic.h>

void KernelPanic(char* message){
    Error("Kernel abort : %s", message);
    Error("Please self restart the computer to continue");
    StopAllCPU();
}