#include "syscall.h"

static uint64_t mutexSyscall;

extern "C" void SyscallInt_Handler(InterruptStack* Registers, uint64_t CoreID){
    Atomic::atomicSpinlock(&mutexSyscall, 0);
    Atomic::atomicLock(&mutexSyscall, 0);

    uint64_t syscall = (uint64_t)Registers->rax;
    uint64_t arg0 = (uint64_t)Registers->rdi;
    uint64_t arg1 = (uint64_t)Registers->rsi;
    uint64_t arg2 = (uint64_t)Registers->rdx;
    uint64_t arg3 = (uint64_t)Registers->r10;
    uint64_t arg4 = (uint64_t)Registers->r8;
    uint64_t arg5 = (uint64_t)Registers->r9;

    uint64_t returnValue = 0;

    switch(syscall){
        case 0x00: //Get Kernel Info
            returnValue = GetKernelInfo(arg0, (void*)arg1);
            break;
        case 0x01:
            globalLogs->Message("%x", arg0);
            break;
        default:
            returnValue = 0;
            break;
    }

    Registers->rax = (void*)returnValue;

    Atomic::atomicUnlock(&mutexSyscall, 0);    
}

int GetKernelInfo(uint64_t InfoID, void* Address){
    switch(InfoID){
        case 0x00: //Get FrameBuffer
            memcpy(Address, globalGraphics->framebuffer, sizeof(Framebuffer));
            return 1; 
            break;
        default:
            return 0; 
    }    
}
