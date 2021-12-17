#pragma once
#include "../main/kernelInit.h"

namespace KernelIPC{
    void Initialize();
    void CreatTask(void* EntryPoint, uint16_t Index);

    extern "C" uint64_t DoSyscall(uint64_t syscall, uint64_t arg0, uint64_t arg1, uint64_t arg2, uint64_t arg3, uint64_t arg4, uint64_t arg5);

    void LogHandler(uint64_t type, char* str);

    void ReadFile(FileSystem::File* file, uint64_t start, size_t size, void* buffer);
    void WriteFile(FileSystem::File* file, uint64_t start, size_t size, void* buffer);
    void OpenFile(char* filePath, char* mode, FileSystem::File* file);
}

//IPC static index
#define IPC_Sys_LogHandler 0x0
#define IPC_Sys_ReadFile 0x1
#define IPC_Sys_WriteFile 0x2
#define IPC_Sys_OpenFile 0x3