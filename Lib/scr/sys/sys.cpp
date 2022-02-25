#include "sys.h"

namespace sys{
    KResult CreatShareSpace(size_t size, uint64_t* virtualAddressPointer, uint64_t* keyPointer, bool ReadOnly, uint8_t Priviledge){
        return Syscall_40(Sys_CreatShareMemory, size, virtualAddressPointer, keyPointer, ReadOnly, Priviledge);
    }

    KResult GetShareSpace(uint64_t key, uint64_t* virtualAddressPointer, uint8_t Priviledge){
        return Syscall_24(Sys_GetShareMemory, key, virtualAddressPointer, Priviledge);
    }

    KResult FreeShareSpace(void* virtualAddress){
        return Syscall_8(Sys_FreeShareMemory, virtualAddress);
    }     

    KResult Fork(thread_t* task, parameters_t* param){
        return Syscall_16(Sys_Fork, task, param);
    }

    KResult Exit(uint64_t errorCode){
        return Syscall_16(Sys_Exit, errorCode);
    }

    //TODO pause and unpause

    KResult Map(void* addressVirtual){
        return Syscall_24(Sys_Map, addressVirtual, false, 0);
    }

    KResult Map(void* addressVirtual, void* addressPhysical){
        return Syscall_24(Sys_Map, addressVirtual, true, addressPhysical);
    }

    KResult Unmap(void* addressVirtual){
        return Syscall_8(Sys_Unmap, addressVirtual);
    }
}