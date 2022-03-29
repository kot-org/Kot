#include "sys.h"

KResult SYS_CreatShareSpace(kthread_t self, size_t size, uint64_t* virtualAddressPointer, uint64_t* keyPointer, bool ReadOnly){
    SYS_Pause(self);
    return Syscall_40(KSys_CreatShareMemory, self, size, virtualAddressPointer, keyPointer, ReadOnly);
}

KResult SYS_GetShareSpace(kthread_t self, uint64_t key, uint64_t* virtualAddressPointer){
    return Syscall_24(KSys_GetShareMemory, self, key, virtualAddressPointer);
}

KResult SYS_FreeShareSpace(kthread_t self, void* virtualAddress){
    return Syscall_16(KSys_FreeShareMemory, self, virtualAddress);
}

KResult SYS_Fork(kthread_t task, struct parameters_t* param){
    return Syscall_16(KSys_Fork, task, param);
}

KResult SYS_Exit(kthread_t self, uint64_t errorCode){
    return Syscall_16(KSys_Exit, self, errorCode);
}

KResult SYS_Pause(kthread_t self){
    return Syscall_8(KSys_Pause, self);
}

KResult SYS_Unpause(kthread_t self){
    return Syscall_8(KSys_UnPause, self);
}

KResult SYS_Map(kthread_t self, void* addressVirtual){
    return Syscall_32(KSys_Map, self, addressVirtual, false, 0);
}

// KResult SYS_Map(kthread_t self, void* addressVirtual, void* addressPhysical){
//     return Syscall_32(KSys_Map, self, addressVirtual, true, addressPhysical);
// }

KResult SYS_Unmap(kthread_t self, void* addressVirtual){
    return Syscall_16(KSys_Unmap, self, addressVirtual);
}