#ifndef KOT_SYS_H
#define KOT_SYS_H

#include <stdint.h>
#include <kot/types.h>
#include <kot/thread.h>
#include <kot/syscall.h>

#if defined(__cplusplus)
extern "C" struct kot_SpecificData_t KotSpecificData;
#else
extern struct kot_SpecificData_t KotSpecificData;
#endif

#if defined(__cplusplus)
extern "C" {
#endif

#define EXEC_FLAGS_SHELL_DISABLED (1 << 0)

struct kot_SpecificData_t{
    /* Memory */
    uint64_t MMapPageSize;
    /* Heap */
    uint64_t HeapLocation;
    /* UISD */
    kot_thread_t UISDHandler;
    kot_process_t UISDHandlerProcess;
    /* FreeMemorySpace */
    void* FreeMemorySpace;
    /* VFS */
    kot_thread_t VFSHandler;
}__attribute__((aligned(0x1000)));

struct kot_SelfData{
    /* Self Info */
    kot_process_t ThreadKey;
    kot_process_t ProcessKey;
    uint64_t PID;
    uint64_t PPID;
    uint64_t TID;
    uint64_t ExternalData_T; /* Thread external data */
    uint64_t ExternalData_P; /* Process external data */
    uint64_t Priviledge; /* Thread priviledge */

    /* Process Creator Info */
    uint64_t PID_PCI;
    uint64_t PPID_PCI;
    uint64_t TID_PCI;
    uint64_t ExternalData_P_PCI; /* Process external data */
    uint64_t Priviledge_PCI; /* Thread priviledge */

    /* Thread Launcher Info */
    uint64_t PID_TLI;
    uint64_t PPID_TLI;
    uint64_t TID_TLI;
    uint64_t ExternalData_P_TLI; /* Process external data */
    uint64_t Priviledge_TLI; /* Thread priviledge */
}__attribute__((packed));

struct kot_ShareDataWithArguments_t{
    size64_t Size;
    void* Data;
    uint8_t ParameterPosition;
}__attribute__((packed));

enum kot_DataType{
    DataTypeUnknow              = 0x0,
    DataTypeThread              = 0x1,
    DataTypeProcess             = 0x2,
    DataTypeEvent               = 0x3,
    DataTypeSharedMemory        = 0x4,
};

enum kot_EventType{
    EventTypeIRQLines           = 0x0,
    EventTypeIRQ                = 0x1,
    EventTypeIPC                = 0x2,
};

enum kot_Priviledge{
    PriviledgeDriver            = 0x1,
    PriviledgeService           = 0x2,
    PriviledgeApp               = 0x3,
    PriviledgeMax              = 0x4,
};

enum kot_MemoryFieldType{
    MemoryFieldTypeUnknow       = 0x0,
    MemoryFieldTypeShareSpaceRW = 0x1,
    MemoryFieldTypeShareSpaceRO = 0x2,
    MemoryFieldTypeSendSpaceRO  = 0x3,
};

#define ExecutionTypeQueu       (1 << 0)
#define ExecutionTypeOneshot    (1 << 1)
#define ExecutionTypeAwait      (1 << 2)
#define ExecutionTypeClose      (1 << 3)

typedef uint64_t kot_execution_type_t;

enum kot_AllocationType{
    AllocationTypeBasic                 = 0x0,
    AllocationTypePhysical              = 0x1,
    AllocationTypePhysicalContiguous    = 0x2,
};




KResult kot_Sys_CreateMemoryField(kot_process_t self, size64_t size, void** virtualAddressPointer, kot_key_mem_t* keyPointer, enum kot_MemoryFieldType type);
KResult kot_Sys_AcceptMemoryField(kot_process_t self, kot_key_mem_t key, void** virtualAddressPointer);
KResult kot_Sys_CloseMemoryField(kot_process_t self, kot_key_mem_t key, void* address);
KResult kot_Sys_GetInfoMemoryField(kot_key_mem_t key, uint64_t* typePointer, size64_t* sizePointer);
KResult kot_Sys_CreateProc(kot_process_t* key, enum kot_Priviledge privilege, uint64_t data);
KResult kot_Sys_Fork(kot_process_t* child);
KResult kot_Sys_CloseProc();
KResult kot_Sys_Close(uint64_t errorCode);
KResult kot_Sys_Exit(uint64_t errorCode);
KResult kot_Sys_Pause(bool force);
KResult kot_Sys_Unpause(kot_thread_t self);
KResult kot_Sys_Map(kot_process_t self, void** addressVirtual, enum kot_AllocationType type, void** addressPhysical, size64_t* size, bool findFree);
KResult kot_Sys_Unmap(kot_process_t self, void* addressVirtual, size64_t size);
void* kot_Sys_GetPhysical(void* addressVirtual);
KResult kot_Sys_Event_Create(kot_event_t* self);
KResult kot_Sys_Event_Bind(kot_event_t self, kot_thread_t task, bool IgnoreMissedEvents);
KResult kot_Sys_Event_Unbind(kot_event_t self, kot_thread_t task);
KResult kot_Sys_Event_Trigger(kot_event_t self, struct kot_arguments_t* parameters);
KResult kot_Sys_Event_Close();
KResult kot_Sys_CreateThread(kot_process_t self, void* entryPoint, enum kot_Priviledge privilege, uint64_t externalData, kot_thread_t* result); // TODO make external data field
KResult kot_Sys_CreateThreadWithoutAutoInit(kot_process_t self, void* entryPoint, enum kot_Priviledge privilege, uint64_t externalData, kot_thread_t* result); // TODO make external data field
KResult kot_Sys_Duplicatethread(kot_process_t parent, kot_thread_t source, kot_thread_t* self);
KResult kot_Sys_ExecThread(kot_thread_t self, struct kot_arguments_t* parameters, kot_execution_type_t type, struct kot_ShareDataWithArguments_t* data);
KResult kot_Sys_Keyhole_CloneModify(kot_key_t source, kot_key_t* destination, kot_process_t target, uint64_t flags, enum kot_Priviledge privilidge);
KResult kot_Sys_Keyhole_Verify(kot_key_t self, enum kot_DataType type, kot_process_t* target, uint64_t* flags, uint64_t* priviledge);
KResult kot_Sys_Thread_Info_Get(kot_thread_t thread, uint64_t arg, uint64_t* value);
KResult kot_Sys_SetTCB(kot_thread_t thread, void* pointer);
KResult kot_Sys_Logs(char* message, size64_t size);

void kot_Sys_Schedule();


kot_thread_t kot_Sys_GetThread();
kot_process_t kot_Sys_GetProcess();
uint64_t kot_Sys_GetPID();
uint64_t kot_Sys_GetPPID();
uint64_t kot_Sys_GetTID();
uint64_t kot_Sys_GetExternalDataThread();
uint64_t kot_Sys_GetExternalDataProcess();
uint64_t kot_Sys_GetPriviledgeThread();

uint64_t kot_Sys_GetPIDThreadCreator();
uint64_t kot_Sys_GetPPIDThreadCreator();
uint64_t kot_Sys_GetTIDThreadCreator();
uint64_t kot_Sys_GetExternalDataProcessCreator();
uint64_t kot_Sys_GetPriviledgeThreadCreator();

uint64_t kot_Sys_GetPIDThreadLauncher();
uint64_t kot_Sys_GetPPIDThreadLauncher();
uint64_t kot_Sys_GetTIDThreadLauncher();
uint64_t kot_Sys_GetExternalDataProcessLauncher();
uint64_t kot_Sys_GetPriviledgeThreadLauncher();

KResult kot_Printlog(char* message);

#if defined(__cplusplus)
} 
#endif

#endif