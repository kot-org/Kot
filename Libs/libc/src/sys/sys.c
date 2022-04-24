#include <kot/sys.h>

KResult SYS_CreatShareSpace(kthread_t self, size_t size, uint64_t* virtualAddressPointer, uint64_t* keyPointer, bool ReadOnly){
    KResult ReturnValue;
    Syscall_40(KSys_CreatShareMemory, self, size, virtualAddressPointer, keyPointer, ReadOnly);
    return ReturnValue;
}

KResult SYS_GetShareSpace(kthread_t self, uint64_t key, uint64_t* virtualAddressPointer){
    KResult ReturnValue;
    Syscall_24(KSys_GetShareMemory, self, key, virtualAddressPointer);
    return ReturnValue;
}

KResult SYS_FreeShareSpace(kthread_t self, void* virtualAddress){
    KResult ReturnValue;
    Syscall_16(KSys_FreeShareMemory, self, virtualAddress);
    return ReturnValue;
}

KResult SYS_Fork(kthread_t task, struct parameters_t* param){
    KResult ReturnValue;
    Syscall_16(KSys_Fork, task, param);
    return ReturnValue;
}

KResult SYS_Exit(kthread_t self, uint64_t errorCode){
    KResult ReturnValue;
    Syscall_16(KSys_Exit, self, errorCode);
    return ReturnValue;
}

KResult SYS_Pause(kthread_t self){
    KResult ReturnValue;
    Syscall_8(KSys_Pause, self);
    return ReturnValue;
}

KResult SYS_Unpause(kthread_t self){
    KResult ReturnValue;
    Syscall_8(KSys_UnPause, self);
    return ReturnValue;
}

KResult SYS_Map(kthread_t self, uint64_t* addressVirtual, bool isPhysical, void* addressPhysical, size_t size, bool findFree){
    KResult ReturnValue;
    Syscall_48(KSys_Map, self, addressVirtual, isPhysical, addressPhysical, size, findFree);
    return ReturnValue;
}

KResult SYS_Unmap(kthread_t self, void* addressVirtual, size_t size){
    KResult ReturnValue;
    Syscall_24(KSys_Unmap, self, addressVirtual, size);
    return ReturnValue;
}