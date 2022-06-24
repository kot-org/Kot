#include <kot/sys.h>

__attribute__((section(".KotSpecificData"))) struct KotSpecificData_t KotSpecificData;

KResult SYS_CreateShareSpace(kprocess_t self, size_t size, uintptr_t* virtualAddressPointer, ksmem_t* keyPointer, uint64_t flags){
    return Syscall_40(KSys_CreateShareMemory, self, size, virtualAddressPointer, keyPointer, flags);
}

KResult SYS_GetShareSpace(kprocess_t self, ksmem_t key, uintptr_t* virtualAddressPointer){
    return Syscall_24(KSys_GetShareMemory, self, key, virtualAddressPointer);
}

KResult SYS_FreeShareSpace(kprocess_t self, ksmem_t key, uintptr_t address){
    return Syscall_24(KSys_FreeShareMemory, self, key, address);
}

KResult SYS_ShareDataUsingStackSpace(kthread_t self, uint64_t address, size_t size, uint64_t* clientAddress){
    return Syscall_32(KSys_ShareDataUsingStackSpace, self, address, size, clientAddress);
}

KResult Sys_CIP(kthread_t task, struct parameters_t* param){
    return Syscall_16(KSys_CIP, task, param);
}

KResult Sys_CreateProc(kprocess_t* key, enum Priviledge privilege, uint64_t data){
    return Syscall_24(KSys_CreateProc, key, privilege, data);
}

KResult Sys_CloseProc(){
    return Syscall_0(KSys_CloseProc);
}

KResult SYS_Exit(kthread_t self, uint64_t errorCode){
    return Syscall_16(KSys_Exit, self, errorCode);
}

KResult SYS_Pause(kthread_t self){
    return Syscall_8(KSys_Pause, self);
}

KResult SYS_Unpause(kprocess_t self){
    return Syscall_8(KSys_UnPause, self);
}

KResult SYS_Map(kprocess_t self, uint64_t* addressVirtual, bool isPhysical, uintptr_t* addressPhysical, size_t* size, bool findFree){
    return Syscall_48(KSys_Map, self, addressVirtual, isPhysical, addressPhysical, size, findFree);
}

KResult SYS_Unmap(kthread_t self, uintptr_t addressVirtual, size_t size){
    return Syscall_24(KSys_Unmap, self, addressVirtual, size);
}

KResult Sys_Event_Create(kevent_t* self){
    return Syscall_8(KSys_Event_Create, self);
}

KResult Sys_Event_Bind(kevent_t self, kthread_t task, uint8_t vector, bool IgnoreMissedEvents){
    return Syscall_32(KSys_Event_Bind, self, task, vector, IgnoreMissedEvents);
}

KResult Sys_Event_Unbind(kevent_t self, kthread_t task, uint8_t vector){
    return Syscall_24(KSys_Event_Unbind, self, task, vector);
}

KResult Sys_Event_Trigger(kevent_t self, struct parameters_t* parameters){
    return Syscall_16(KSys_Event_Trigger, self, parameters);
}

KResult Sys_Event_Close(){
    return Syscall_0(KSys_Event_Close);
}

KResult Sys_CreateThread(kprocess_t self, uintptr_t entryPoint, enum Priviledge privilege, uint64_t data, kthread_t* result){
    return Syscall_40(KSys_CreateThread, self, entryPoint, privilege, data, result);
}

KResult Sys_DuplicateThread(kprocess_t parent, kthread_t source, uint64_t data, kthread_t* self){
    return Syscall_32(KSys_DuplicateThread, parent, source, data, self);
}

KResult Sys_ExecThread(kthread_t self, struct parameters_t* parameters){
    return Syscall_16(KSys_ExecThread, self, parameters);
}

KResult Sys_Logs(char* message, size_t size){
    return Syscall_16(KSys_Logs, message, size);
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

KResult Printlog(char* message){
    return Sys_Logs(message, strlen(message));
}
