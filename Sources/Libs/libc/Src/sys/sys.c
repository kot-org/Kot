#include <kot/sys.h>

__attribute__((section(".KotSpecificData"))) struct KotSpecificData_t KotSpecificData;

KResult Sys_CreateMemoryField(process_t self, size_t size, uintptr_t* virtualAddressPointer, ksmem_t* keyPointer, enum MemoryFieldType type){
    return Syscall_40(KSys_CreateMemoryField, self, size, virtualAddressPointer, keyPointer, type);
}

KResult Sys_AcceptMemoryField(process_t self, ksmem_t key, uintptr_t* virtualAddressPointer){
    return Syscall_24(KSys_AcceptMemoryField, self, key, virtualAddressPointer);
}

KResult Sys_FreeMemoryField(process_t self, ksmem_t key, uintptr_t address){
    return Syscall_24(KSys_FreeMemoryField, self, key, address);
}

KResult Sys_GetInfoMemoryField(ksmem_t key, uint64_t* typePointer, size_t* sizePointer){
    return Syscall_24(KSys_GetTypeMemoryField, key, typePointer, sizePointer);
}

KResult SYS_ShareDataUsingStackSpace(thread self, uint64_t address, size_t size, uint64_t* clientAddress){
    return Syscall_32(KSys_ShareDataUsingStackSpace, self, address, size, clientAddress);
}

KResult Sys_CreateProc(process_t* key, enum Priviledge privilege, uint64_t data){
    return Syscall_24(KSys_CreateProc, key, privilege, data);
}

KResult Sys_CloseProc(){
    return Syscall_0(KSys_CloseProc);
}

KResult SYS_Exit(thread self, uint64_t errorCode){
    return Syscall_16(KSys_Exit, self, errorCode);
}

KResult SYS_Pause(thread self){
    return Syscall_8(KSys_Pause, self);
}

KResult SYS_Unpause(process_t self){
    return Syscall_8(KSys_UnPause, self);
}

KResult SYS_Map(process_t self, uint64_t* addressVirtual, bool isPhysical, uintptr_t* addressPhysical, size_t* size, bool findFree){
    return Syscall_48(KSys_Map, self, addressVirtual, isPhysical, addressPhysical, size, findFree);
}

KResult SYS_Unmap(thread self, uintptr_t addressVirtual, size_t size){
    return Syscall_24(KSys_Unmap, self, addressVirtual, size);
}

KResult Sys_Event_Create(kevent_t* self){
    return Syscall_8(KSys_Event_Create, self);
}

KResult Sys_Event_Bind(kevent_t self, thread task, uint8_t vector, bool IgnoreMissedEvents){
    return Syscall_32(KSys_Event_Bind, self, task, vector, IgnoreMissedEvents);
}

KResult Sys_Event_Unbind(kevent_t self, thread task, uint8_t vector){
    return Syscall_24(KSys_Event_Unbind, self, task, vector);
}

KResult Sys_Event_Trigger(kevent_t self, struct arguments_t* parameters){
    return Syscall_16(KSys_Event_Trigger, self, parameters);
}

KResult Sys_Event_Close(){
    return Syscall_0(KSys_Event_Close);
}

KResult Sys_Createthread(process_t self, uintptr_t entryPoint, enum Priviledge privilege, uint64_t data, thread* result){
    return Syscall_40(KSys_CreateThread, self, entryPoint, privilege, data, result);
}

KResult Sys_Duplicatethread(process_t parent, thread source, uint64_t data, thread* self){
    return Syscall_32(KSys_DuplicateThread, parent, source, data, self);
}

KResult Sys_Execthread(thread self, struct arguments_t* parameters, enum ExecutionType type, struct ShareDataWithArguments_t* data){
    return Syscall_32(KSys_ExecThread, self, parameters, type, data);
}

KResult Sys_Keyhole_CloneModify(key_t source, key_t* destination, process_t target, uint64_t flags){
    return Syscall_32(KSys_Keyhole_CloneModify, source, destination, target, flags);
}

KResult Sys_Keyhole_Verify(key_t self, enum DataType type, process_t* target, uint64_t* flags){
    return Syscall_32(KSys_Keyhole_Verify, self, type, target, flags);
}

KResult Sys_Logs(char* message, size_t size){
    return Syscall_16(KSys_Logs, message, size);
}


KResult Sys_GetthreadKey(thread* self){
    /* Get Self Data */
    thread key;
    asm("mov %%gs:0, %0":"=r"(key));
    *self = key; 
    return KSUCCESS;
}

KResult Sys_GetProcessKey(process_t* self){
    /* Get Self Data */
    process_t key;
    asm("mov %%gs:8, %0":"=r"(key));
    *self = key; 
    return KSUCCESS;
}

KResult Printlog(char* message){
    return Sys_Logs(message, strlen(message));
}
