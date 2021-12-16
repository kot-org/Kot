#pragma once
#include "../main/kernelInit.h"

namespace KernelIPC{
    #define ReturnSyscall 0x04
    void Initialize();
    void CreatTask(void* EntryPoint, uint16_t Index);

    extern "C" uint64_t DoSyscall(uint64_t syscall, uint64_t arg0, uint64_t arg1, uint64_t arg2, uint64_t arg3, uint64_t arg4, uint64_t arg5);

    void LogHandler(uint64_t type, char* str);

}
