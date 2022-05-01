#include <kot/sys.h>

KResult SYS_CreatShareSpace(kprocess_t self, size_t size, uintptr_t* virtualAddressPointer, ksmem_t* keyPointer, uint64_t flags){
    KResult ReturnValue;
    Syscall_40(KSys_CreatShareMemory, self, size, virtualAddressPointer, keyPointer, flags);
    return ReturnValue;
}

KResult SYS_GetShareSpace(kprocess_t self, ksmem_t key, uintptr_t* virtualAddressPointer){
    KResult ReturnValue;
    Syscall_24(KSys_GetShareMemory, self, key, virtualAddressPointer);
    return ReturnValue;
}

KResult SYS_FreeShareSpace(kprocess_t self, ksmem_t key, uintptr_t address){
    KResult ReturnValue;
    Syscall_24(KSys_FreeShareMemory, self, key, address);
    return ReturnValue;
}

KResult SYS_Fork(kthread_t task, struct parameters_t* param){
    KResult ReturnValue;
    Syscall_16(KSys_Fork, task, param);
    return ReturnValue;
}

KResult Sys_CreatProc(kprocess_t* key, uint8_t privilege, uint64_t data){
    KResult ReturnValue;
    Syscall_24(KSys_CreatProc, key, privilege, data);
    return ReturnValue;
}

KResult Sys_CloseProc(){
    KResult ReturnValue;
    Syscall_0(KSys_CloseProc);
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

KResult SYS_Unpause(kprocess_t self){
    KResult ReturnValue;
    Syscall_8(KSys_UnPause, self);
    return ReturnValue;
}

KResult SYS_Map(kprocess_t self, uint64_t* addressVirtual, bool isPhysical, void* addressPhysical, size_t size, bool findFree){
    KResult ReturnValue;
    Syscall_48(KSys_Map, self, addressVirtual, isPhysical, addressPhysical, size, findFree);
    return ReturnValue;
}

KResult SYS_Unmap(kthread_t self, void* addressVirtual, size_t size){
    KResult ReturnValue;
    Syscall_24(KSys_Unmap, self, addressVirtual, size);
    return ReturnValue;
}

KResult Sys_Event_Creat(kevent_t* self, enum EventType type, uint8_t vector){
    KResult ReturnValue;
    Syscall_16(KSys_Event_Creat, self, vector);
    return ReturnValue;
}

KResult Sys_Event_Bind(kevent_t self, uint8_t vector){
    KResult ReturnValue;
    Syscall_16(KSys_Event_Bind, self, vector);
    return ReturnValue;
}

KResult Sys_Event_Unbind(kevent_t self){
    KResult ReturnValue;
    Syscall_8(KSys_Event_Unbind, self);
    return ReturnValue;
}

KResult Sys_Event_Trigger(kevent_t self, void* dataAddress, size_t dataSize){
    KResult ReturnValue;
    Syscall_24(KSys_Event_Trigger, self, dataAddress, dataSize);
    return ReturnValue;
}

KResult Sys_CreatThread(kprocess_t self, void* entryPoint, uint8_t privilege, uint64_t data, kthread_t* result){
    KResult ReturnValue;
    Syscall_40(KSys_CreatThread, self, entryPoint, privilege, data, result);
    return ReturnValue;
}

KResult Sys_DuplicateThread(kprocess_t parent, kthread_t source, uint64_t data, kthread_t* self){
    KResult ReturnValue;
    Syscall_32(KSys_DuplicateThread, parent, source, data, self);
    return ReturnValue;
}

KResult Sys_ExecThread(kthread_t self, struct parameters_t* parameters){
    KResult ReturnValue;
    Syscall_16(KSys_ExecThread, self, parameters);
    return ReturnValue;
}


KResult SYS_GetThreadKey(kthread_t* self){
    /* Get Self Data */
    kthread_t key;
    asm("mov %%gs:0, %0":"=r"(key));
    *self = key; 
    return KSUCCESS;
}

KResult SYS_GetProcessKey(kprocess_t* self){
    /* Get Self Data */
    kprocess_t key;
    asm("mov %%gs:8, %0":"=r"(key));
    *self = key; 
    return KSUCCESS;
}
