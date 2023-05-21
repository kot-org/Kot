#include <kot/sys.h>

extern "C" {

__attribute__((section(".KotSpecificData"))) struct kot_KotSpecificData_t KotSpecificData;

KResult kot_Sys_CreateMemoryField(kot_process_t self, size64_t size, uintptr_t* virtualAddressPointer, kot_ksmem_t* keyPointer, enum kot_MemoryFieldType type){
    return Syscall_40(KSys_CreateMemoryField, self, size, virtualAddressPointer, keyPointer, type);
}

KResult kot_Sys_AcceptMemoryField(kot_process_t self, kot_ksmem_t key, uintptr_t* virtualAddressPointer){
    return Syscall_24(KSys_AcceptMemoryField, self, key, virtualAddressPointer);
}

KResult kot_Sys_CloseMemoryField(kot_process_t self, kot_ksmem_t key, uintptr_t address){
    return Syscall_24(KSys_CloseMemoryField, self, key, address);
}

KResult kot_Sys_GetInfoMemoryField(kot_ksmem_t key, uint64_t* typePointer, size64_t* sizePointer){
    return Syscall_24(KSys_GetTypeMemoryField, key, typePointer, sizePointer);
}

KResult kot_Sys_CreateProc(kot_process_t* key, enum kot_Priviledge privilege, uint64_t data){
    return Syscall_24(KSys_CreateProc, key, privilege, data);
}

KResult kot_Sys_Fork(kot_process_t* child){
    return Syscall_8(KSys_Fork, child);
}

KResult kot_Sys_CloseProc(){
    return Syscall_0(KSys_CloseProc);
}

KResult kot_Sys_Close(uint64_t errorCode){
    return Syscall_8(KSys_Close, errorCode);
}

KResult kot_Sys_Exit(uint64_t errorCode){
    return Syscall_8(KSys_Exit, errorCode);
}

KResult kot_Sys_Pause(bool force){
    return Syscall_8(KSys_Pause, force);
}

KResult kot_Sys_Unpause(kot_thread_t self){
    return Syscall_8(KSys_UnPause, self);
}

KResult kot_Sys_Map(kot_process_t self, uint64_t* addressVirtual, enum kot_AllocationType type, uintptr_t* addressPhysical, size64_t* size, bool findFree){
    return Syscall_48(KSys_Map, self, addressVirtual, type, addressPhysical, size, findFree);
}

KResult kot_Sys_Unmap(kot_thread_t self, uintptr_t addressVirtual, size64_t size){
    return Syscall_24(KSys_Unmap, self, addressVirtual, size);
}

uintptr_t kot_Sys_GetPhysical(uintptr_t addressVirtual){
    return Syscall_8(KSys_GetPhysical, addressVirtual);
}

KResult kot_Sys_Event_Create(kot_event_t* self){
    return Syscall_8(KSys_Event_Create, self);
}

KResult kot_Sys_Event_Bind(kot_event_t self, kot_thread_t task, bool IgnoreMissedEvents){
    return Syscall_24(KSys_Event_Bind, self, task, IgnoreMissedEvents);
}

KResult kot_Sys_Event_Unbind(kot_event_t self, kot_thread_t task){
    return Syscall_16(KSys_Event_Unbind, self, task);
}

KResult kot_Sys_kot_event_trigger(kot_event_t self, struct kot_arguments_t* parameters){
    return Syscall_16(KSys_Event_Trigger, self, parameters);
}

KResult kot_Sys_Event_Close(){
    return Syscall_0(KSys_Event_Close);
}

KResult kot_Sys_CreateThread(kot_process_t self, uintptr_t entryPoint, enum kot_Priviledge privilege, uint64_t externalData, kot_thread_t* result){
    KResult Status = Syscall_40(KSys_CreateThread, self, entryPoint, privilege, externalData, result);
    if(Status != KSUCCESS) return Status;
    Status = kot_InitializeThread(*result);
    return Status;
}

KResult kot_Sys_CreateThreadWithoutAutoInit(kot_process_t self, uintptr_t entryPoint, enum kot_Priviledge privilege, uint64_t externalData, kot_thread_t* result){
    return Syscall_40(KSys_CreateThread, self, entryPoint, privilege, externalData, result);
}

KResult kot_Sys_Duplicatethread(kot_process_t parent, kot_thread_t source, kot_thread_t* self){
    return Syscall_24(KSys_DuplicateThread, parent, source, self);
}

KResult kot_Sys_ExecThread(kot_thread_t self, struct kot_arguments_t* parameters, enum kot_ExecutionType type, struct kot_ShareDataWithArguments_t* data){
    return Syscall_32(KSys_ExecThread, self, parameters, type, data);
}

KResult kot_Sys_Keyhole_CloneModify(kot_key_t source, kot_key_t* destination, kot_process_t target, uint64_t flags, enum kot_Priviledge privilidge){
    return Syscall_40(KSys_Keyhole_CloneModify, source, destination, target, flags, privilidge);
}

KResult kot_Sys_Keyhole_Verify(kot_key_t self, enum kot_DataType type, kot_process_t* target, uint64_t* flags, uint64_t* priviledge){
    return Syscall_40(KSys_Keyhole_Verify, self, type, target, flags, priviledge);
}

KResult kot_Sys_SetTCB(kot_thread_t thread, uintptr_t pointer){
    return Syscall_16(KSys_TCB_Set, thread, (uint64_t)pointer);
}

KResult kot_Sys_Thread_Info_Get(kot_thread_t thread, uint64_t arg, uint64_t* value){
    return Syscall_24(KSys_Thread_Info_Get, thread, (uint64_t)arg, (uint64_t)value);
}

KResult kot_Sys_Logs(char* message, size64_t size){
    return Syscall_16(KSys_Logs, message, size);
}

void kot_Sys_Schedule(){
    asm("int $0x41");
}

kot_thread_t kot_Sys_GetThread(){
    /* Get Self Data */
    uint64_t self = NULL;
    asm("mov %%gs:0x0, %0":"=r"(self));
    return self;
}

kot_process_t kot_Sys_GetProcess(){
    /* Get Self Data */
    uint64_t self;
    asm("mov %%gs:0x8, %0":"=r"(self));
    return self;
}

uint64_t kot_Sys_GetPID(){
    /* Get Self Data */
    uint64_t PID = NULL;
    asm("mov %%gs:0x10, %0":"=r"(PID));
    return PID;
}

uint64_t kot_Sys_GetPPID(){
    /* Get Self Data */
    uint64_t PPID = NULL;
    asm("mov %%gs:0x18, %0":"=r"(PPID));
    return PPID;
}

uint64_t kot_Sys_GetTID(){
    /* Get Self Data */
    uint64_t TID = NULL;
    asm("mov %%gs:0x20, %0":"=r"(TID));
    return TID;
}

uint64_t kot_Sys_GetExternalDataThread(){
    /* Get Self Data */
    uint64_t ExternalData = NULL;
    asm("mov %%gs:0x28, %0":"=r"(ExternalData));
    return ExternalData;
}

uint64_t kot_Sys_GetExternalDataProcess(){
    /* Get Self Data */
    uint64_t ExternalData = NULL;
    asm("mov %%gs:0x30, %0":"=r"(ExternalData));
    return ExternalData;
}

uint64_t kot_Sys_GetPriviledgeThread(){
    /* Get Self Data */
    uint64_t Priviledge = NULL;
    asm("mov %%gs:0x38, %0":"=r"(Priviledge));
    return Priviledge;
}

uint64_t kot_Sys_GetPIDThreadCreator(){
    /* Get Self Data */
    uint64_t PID = NULL;
    asm("mov %%gs:0x40, %0":"=r"(PID));
    return PID;
}

uint64_t kot_Sys_GetPPIDThreadCreator(){
    /* Get Self Data */
    uint64_t PPID = NULL;
    asm("mov %%gs:0x48, %0":"=r"(PPID));
    return PPID;
}

uint64_t kot_Sys_GetTIDThreadCreator(){
    /* Get Self Data */
    uint64_t TID = NULL;
    asm("mov %%gs:0x50, %0":"=r"(TID));
    return TID;
}

uint64_t kot_Sys_GetExternalDataProcessCreator(){
    /* Get Self Data */
    uint64_t ExternalData = NULL;
    asm("mov %%gs:0x58, %0":"=r"(ExternalData));
    return ExternalData;
}

uint64_t kot_Sys_GetPriviledgeThreadCreator(){
    /* Get Self Data */
    uint64_t Priviledge = NULL;
    asm("mov %%gs:0x60, %0":"=r"(Priviledge));
    return Priviledge;
}

uint64_t kot_Sys_GetPIDThreadLauncher(){
    /* Get Self Data */
    uint64_t PID = NULL;
    asm("mov %%gs:0x68, %0":"=r"(PID));
    return PID;
}

uint64_t kot_Sys_GetPPIDThreadLauncher(){
    /* Get Self Data */
    uint64_t PPID = NULL;
    asm("mov %%gs:0x70, %0":"=r"(PPID));
    return PPID;
}

uint64_t kot_Sys_GetTIDThreadLauncher(){
    /* Get Self Data */
    uint64_t TID = NULL;
    asm("mov %%gs:0x78, %0":"=r"(TID));
    return TID;
}

uint64_t kot_Sys_GetExternalDataProcessLauncher(){
    /* Get Self Data */
    uint64_t ExternalData = NULL;
    asm("mov %%gs:0x80, %0":"=r"(ExternalData));
    return ExternalData;
}

uint64_t kot_Sys_GetPriviledgeThreadLauncher(){
    /* Get Self Data */
    uint64_t Priviledge = NULL;
    asm("mov %%gs:0x88, %0":"=r"(Priviledge));
    return Priviledge;
}

}