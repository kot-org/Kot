#include <misc/panic/panic.h>

void KernelPanic(char* message){
    globalLogs->Error("Kernel abort : %s", message);
    globalLogs->Error("Please self restart the computer to continue");
    StopAllCPU();
}