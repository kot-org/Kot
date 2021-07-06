#include "system.h"

bool IsIntInit = false;
bool wait = true;
static uint64_t mutexSyscall;

extern "C" void SyscallEntry(SyscallStack* Registers, uint8_t CoreID){
    Atomic::atomicSpinlock(&mutexSyscall, 0);
    Atomic::atomicLock(&mutexSyscall, 0);

    uint64_t syscall = (uint64_t)Registers->rax;
    uint64_t arg0 = (uint64_t)Registers->rdi;
    uint64_t arg1 = (uint64_t)Registers->rsi;
    uint64_t arg2 = (uint64_t)Registers->rdx;
    uint64_t arg3 = (uint64_t)Registers->r10;
    uint64_t arg4 = (uint64_t)Registers->r8;
    uint64_t arg5 = (uint64_t)Registers->r9;

    switch(syscall){
        case 0x01:
            break;
    }

    //globalGraphics->Print((char*)arg5);

    __asm__ __volatile__ ("pause" : : : "memory");
    Atomic::atomicUnlock(&mutexSyscall, 0);
}

extern "C" uint64_t SystemExit(uint64_t ErrorCode){

}

