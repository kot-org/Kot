#ifndef KOT_SYS_H
#define KOT_SYS_H

#include <stdint.h>
#include <kot/types.h>
#include <kot/syscall.h>

struct KotSpecificData_t{
    /* Memory */
    uint64_t MMapPageSize;
    /* Heap */
    uint64_t HeapLocation;
    /* UISD */
    kot_thread_t UISDHandler;
    kot_process_t UISDHandlerProcess;
    /* FreeMemorySpace */
    uintptr_t FreeMemorySpace;
    /* VFS */
    kot_thread_t VFSHandler;
}__attribute__((aligned(0x1000)));

extern "C" struct KotSpecificData_t KotSpecificData;

namespace Kot{
    struct SelfData{
        /* Self Info */
        kot_process_t ThreadKey;
        kot_process_t ProcessKey;
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




    KResult Sys_CreateMemoryField(kot_process_t self, size64_t size, uintptr_t* virtualAddressPointer, kot_ksmem_t* keyPointer, enum MemoryFieldType type);
    KResult Sys_AcceptMemoryField(kot_process_t self, kot_ksmem_t key, uintptr_t* virtualAddressPointer);
    KResult Sys_CloseMemoryField(kot_process_t self, kot_ksmem_t key, uintptr_t address);
    KResult Sys_GetInfoMemoryField(kot_ksmem_t key, uint64_t* typePointer, size64_t* sizePointer);
    KResult Sys_CreateProc(kot_process_t* key, enum Priviledge privilege, uint64_t data);
    KResult Sys_Fork(kot_process_t* src, kot_process_t* dst);
    KResult Sys_CloseProc();
    KResult Sys_Close(uint64_t errorCode);
    KResult Sys_Exit(uint64_t errorCode);
    KResult Sys_Pause(bool force);
    KResult Sys_Unpause(kot_thread_t self);
    KResult Sys_Map(kot_process_t self, uint64_t* addressVirtual, enum AllocationType type, uintptr_t* addressPhysical, size64_t* size, bool findFree);
    KResult SYS_Unmap(kot_process_t self, uintptr_t addressVirtual, size64_t size);
    uintptr_t Sys_GetPhysical(uintptr_t addressVirtual);
    KResult Sys_Event_Create(kot_event_t* self);
    KResult Sys_Event_Bind(kot_event_t self, kot_thread_t task, bool IgnoreMissedEvents);
    KResult Sys_Event_Unbind(kot_event_t self, kot_thread_t task);
    KResult Sys_kot_event_trigger(kot_event_t self, struct kot_arguments_t* parameters);
    KResult Sys_Event_Close();
    KResult Sys_CreateThread(kot_process_t self, uintptr_t entryPoint, enum Priviledge privilege, uint64_t externalData, kot_thread_t* result); // TODO make external data field
    KResult Sys_Duplicatethread(kot_process_t parent, kot_thread_t source, kot_thread_t* self);
    KResult Sys_ExecThread(kot_thread_t self, struct kot_arguments_t* parameters, enum ExecutionType type, struct ShareDataWithArguments_t* data);
    KResult Sys_Keyhole_CloneModify(kot_key_t source, kot_key_t* destination, kot_process_t target, uint64_t flags, enum Priviledge privilidge);
    KResult Sys_Keyhole_Verify(kot_key_t self, enum DataType type, kot_process_t* target, uint64_t* flags, uint64_t* priviledge);
    KResult Sys_Logs(char* message, size64_t size);

    void Sys_Schedule();


    kot_thread_t Sys_Getthread();
    kot_process_t Sys_GetProcess();
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

}

#endif