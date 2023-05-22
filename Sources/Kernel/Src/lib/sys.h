#pragma once

struct kot_SpecificData_t{
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

extern struct kot_SpecificData_t KotSpecificData;

struct SelfData{
    /* Self Info */
    process_t ThreadKey;
    process_t ProcessKey;
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