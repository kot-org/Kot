#include "sys.h"

namespace sys{
    KResult CreatShareSpace(kthread_t self, size_t size, uint64_t* virtualAddressPointer, uint64_t* keyPointer, bool ReadOnly){
        return Syscall_40(Sys_CreatShareMemory, self, size, virtualAddressPointer, keyPointer, ReadOnly);
    }

    KResult GetShareSpace(kthread_t self, uint64_t key, uint64_t* virtualAddressPointer){
        return Syscall_24(Sys_GetShareMemory, self, key, virtualAddressPointer);
    }

    KResult FreeShareSpace(kthread_t self, void* virtualAddress){
        return Syscall_16(Sys_FreeShareMemory, self, virtualAddress);
    }     

    KResult Fork(kthread_t task, parameters_t* param){
        return Syscall_16(Sys_Fork, task, param);
    }

    KResult Exit(kthread_t self, uint64_t errorCode){
        return Syscall_16(Sys_Exit, self, errorCode);
    }

    KResult Pause(kthread_t self){
        return Syscall_8(Sys_Pause, self);
    }

    KResult Unpause(kthread_t self){
        return Syscall_8(Sys_UnPause, self);
    }

    KResult Map(kthread_t self, void* addressVirtual){
        return Syscall_32(Sys_Map, self, addressVirtual, false, 0);
    }

    KResult Map(kthread_t self, void* addressVirtual, void* addressPhysical){
        return Syscall_32(Sys_Map, self, addressVirtual, true, addressPhysical);
    }

    KResult Unmap(kthread_t self, void* addressVirtual){
        return Syscall_16(Sys_Unmap, self, addressVirtual);
    }
}