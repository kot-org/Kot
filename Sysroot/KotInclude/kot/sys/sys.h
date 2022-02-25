#pragma once

#include <kot/types.h>
#include "list.h"

#define Syscall_48(syscall, arg0, arg1, arg2, arg3, arg4, arg5) (DoSyscall(syscall, (uint64_t)arg0, (uint64_t)arg1, (uint64_t)arg2, (uint64_t)arg3, (uint64_t)arg4, (uint64_t)arg5))
#define Syscall_40(syscall, arg0, arg1, arg2, arg3, arg4) (DoSyscall(syscall, (uint64_t)arg0, (uint64_t)arg1, (uint64_t)arg2, (uint64_t)arg3, (uint64_t)arg4, 0))
#define Syscall_32(syscall, arg0, arg1, arg2, arg3) (DoSyscall(syscall, (uint64_t)arg0, (uint64_t)arg1, (uint64_t)arg2, (uint64_t)arg3, 0, 0))
#define Syscall_24(syscall, arg0, arg1, arg2) (DoSyscall(syscall, (uint64_t)arg0, (uint64_t)arg1, (uint64_t)arg2, 0, 0, 0))
#define Syscall_16(syscall, arg0, arg1) (DoSyscall(syscall, (uint64_t)arg0, (uint64_t)arg1, 0, 0, 0, 0))
#define Syscall_8(syscall, arg0) (DoSyscall(syscall, (uint64_t)arg0, 0, 0, 0, 0, 0))
#define Syscall_0(syscall) (DoSyscall(syscall, 0, 0, 0, 0, 0, 0))

extern "C" uint64_t DoSyscall(uint64_t syscall, uint64_t arg0, uint64_t arg1, uint64_t arg2, uint64_t arg3, uint64_t arg4, uint64_t arg5);

namespace sys{

    void CreatShareSpace();
    void GetShareSpace();   
    void FreeShareSpace();   
}
