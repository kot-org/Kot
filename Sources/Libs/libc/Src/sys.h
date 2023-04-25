#ifndef _SYS_H
#define _SYS_H 1

#include <kot/arch.h>
#include <kot/types.h>
#include <kot/memory.h>
#include <kot/sys/list.h>

#define Syscall_48(syscall, arg0, arg1, arg2, arg3, arg4, arg5) (DoSyscall(syscall, (uint64_t)arg0, (uint64_t)arg1, (uint64_t)arg2, (uint64_t)arg3, (uint64_t)arg4, (uint64_t)arg5))
#define Syscall_40(syscall, arg0, arg1, arg2, arg3, arg4) (DoSyscall(syscall, (uint64_t)arg0, (uint64_t)arg1, (uint64_t)arg2, (uint64_t)arg3, (uint64_t)arg4, 0))
#define Syscall_32(syscall, arg0, arg1, arg2, arg3) (DoSyscall(syscall, (uint64_t)arg0, (uint64_t)arg1, (uint64_t)arg2, (uint64_t)arg3, 0, 0))
#define Syscall_24(syscall, arg0, arg1, arg2) (DoSyscall(syscall, (uint64_t)arg0, (uint64_t)arg1, (uint64_t)arg2, 0, 0, 0))
#define Syscall_16(syscall, arg0, arg1) (DoSyscall(syscall, (uint64_t)arg0, (uint64_t)arg1, 0, 0, 0, 0))
#define Syscall_8(syscall, arg0) (DoSyscall(syscall, (uint64_t)arg0, 0, 0, 0, 0, 0))
#define Syscall_0(syscall) (DoSyscall(syscall, 0, 0, 0, 0, 0, 0))

#if defined(__cplusplus)
extern "C" {
#endif

struct KotSpecificData_t{
    /* Memory */
    uint64_t MMapPageSize;
    /* Heap */
    uint64_t HeapLocation;
    /* UISD */
    thread_t UISDHandler;
    process_t UISDHandlerProcess;
    /* FreeMemorySpace */
    uintptr_t FreeMemorySpace;
    /* VFS */
    thread_t VFSHandler;
}__attribute__((aligned(0x1000)));

extern struct KotSpecificData_t KotSpecificData;

struct SelfData{
    /* Self Info */
    process_t ThreadKey;
    process_t ProcessKey;
    uint64_t PID;
    uint64_t TID;
    uint64_t ExternalData_T; /* Thread external data */
    uint64_t ExternalData_P; /* Process external data */
    uint64_t Priviledge; /* Thread priviledge */

    /* Process Creator Info */
    uint64_t PID_PCI;
    uint64_t TID_PCI;
    uint64_t ExternalData_P_PCI; /* Process external data */
    uint64_t Priviledge_PCI; /* Thread priviledge */

    /* Thread Launcher Info */
    uint64_t PID_TLI;
    uint64_t TID_TLI;
    uint64_t ExternalData_P_TLI; /* Process external data */
    uint64_t Priviledge_TLI; /* Thread priviledge */
}__attribute__((packed));

struct ShareDataWithArguments_t{
    size64_t Size;
    uintptr_t Data;
    uint8_t ParameterPosition;
}__attribute__((packed));

enum DataType{
    DataTypeUnknow              = 0x0,
    DataTypeThread              = 0x1,
    DataTypeProcess             = 0x2,
    DataTypeEvent               = 0x3,
    DataTypeSharedMemory        = 0x4,
};

enum EventType{
    EventTypeIRQLines           = 0x0,
    EventTypeIRQ                = 0x1,
    EventTypeIPC                = 0x2,
};

enum Priviledge{
    PriviledgeDriver            = 0x1,
    PriviledgeService           = 0x2,
    PriviledgeApp               = 0x3,
    PriviledgeMax              = 0x4,
};

enum MemoryFieldType{
    MemoryFieldTypeUnknow       = 0x0,
    MemoryFieldTypeShareSpaceRW = 0x1,
    MemoryFieldTypeShareSpaceRO = 0x2,
    MemoryFieldTypeSendSpaceRO  = 0x3,
};

enum ExecutionType{
    ExecutionTypeQueu           = 0x0,
    ExecutionTypeQueuAwait      = 0x1,
    ExecutionTypeOneshot        = 0x2,
    ExecutionTypeOneshotAwait   = 0x3,
};

enum AllocationType{
    AllocationTypeBasic                 = 0x0,
    AllocationTypePhysical              = 0x1,
    AllocationTypePhysicalContiguous    = 0x2,
};

uint64_t DoSyscall(uint64_t syscall, uint64_t arg0, uint64_t arg1, uint64_t arg2, uint64_t arg3, uint64_t arg4, uint64_t arg5);


KResult Sys_CreateMemoryField(process_t self, size64_t size, uintptr_t* virtualAddressPointer, ksmem_t* keyPointer, enum MemoryFieldType type);
KResult Sys_AcceptMemoryField(process_t self, ksmem_t key, uintptr_t* virtualAddressPointer);
KResult Sys_CloseMemoryField(process_t self, ksmem_t key, uintptr_t address);
KResult Sys_GetInfoMemoryField(ksmem_t key, uint64_t* typePointer, size64_t* sizePointer);
KResult Sys_CreateProc(process_t* key, enum Priviledge privilege, uint64_t data);
KResult Sys_Fork(process_t* src, process_t* dst);
KResult Sys_CloseProc();
KResult Sys_Close(uint64_t errorCode);
KResult Sys_Exit(uint64_t errorCode);
KResult Sys_Pause(bool force);
KResult Sys_Unpause(thread_t self);
KResult Sys_Map(process_t self, uint64_t* addressVirtual, enum AllocationType type, uintptr_t* addressPhysical, size64_t* size, bool findFree);
KResult Sys_Unmap(process_t self, uintptr_t addressVirtual, size64_t size);
uintptr_t Sys_GetPhysical(uintptr_t addressVirtual);
KResult Sys_Event_Create(event_t* self);
KResult Sys_Event_Bind(event_t self, thread_t task, bool IgnoreMissedEvents);
KResult Sys_Event_Unbind(event_t self, thread_t task);
KResult Sys_Event_Trigger(event_t self, struct arguments_t* parameters);
KResult Sys_Event_Close();
KResult Sys_CreateThread(process_t self, uintptr_t entryPoint, enum Priviledge privilege, uint64_t externalData, thread_t* result); // TODO make external data field
KResult Sys_Duplicatethread(process_t parent, thread_t source, thread_t* self);
KResult Sys_ExecThread(thread_t self, struct arguments_t* parameters, enum ExecutionType type, struct ShareDataWithArguments_t* data);
KResult Sys_Keyhole_CloneModify(key_t source, key_t* destination, process_t target, uint64_t flags, enum Priviledge privilidge);
KResult Sys_Keyhole_Verify(key_t self, enum DataType type, process_t* target, uint64_t* flags, uint64_t* priviledge);
KResult Sys_Logs(char* message, size64_t size);

void Sys_Schedule();


thread_t Sys_Getthread();
process_t Sys_GetProcess();
uint64_t Sys_GetPID();
uint64_t Sys_GetTID();
uint64_t Sys_GetExternalDataThread();
uint64_t Sys_GetExternalDataProcess();
uint64_t Sys_GetPriviledgeThread();

uint64_t Sys_GetPIDThreadCreator();
uint64_t Sys_GetTIDThreadCreator();
uint64_t Sys_GetExternalDataProcessCreator();
uint64_t Sys_GetPriviledgeThreadCreator();

uint64_t Sys_GetPIDThreadLauncher();
uint64_t Sys_GetTIDThreadLauncher();
uint64_t Sys_GetExternalDataProcessLauncher();
uint64_t Sys_GetPriviledgeThreadLauncher();

KResult Printlog(char* message);

#if defined(__cplusplus)
} 
#endif

#endif