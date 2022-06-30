#pragma once
#include <arch/arch.h>
#include <kot/types.h>
#include <heap/heap.h>
#include <lib/limits.h>
#include <event/event.h>
#include <keyhole/keyhole.h>

struct TaskQueuNode;
struct Task;
class TaskManager;

#define SelfDataStartAddress vmm_MapAddress(0xff, 0, 0, 0)  
#define SelfDataEndAddress SelfDataStartAddress + sizeof(SelfData)
#define StackTop vmm_MapAddress(0x100, 0, 0, 0)
#define StackBottom SelfDataEndAddress 
#define LockAddress vmm_MapAddress(0xfe, 0, 0, 0) 
#define FreeMemorySpaceAddress vmm_MapAddress(0xfe, 0, 0, 0) 
#define DefaultFlagsKey 0xff
#define ShareMaxIntoStackSpace PAGE_SIZE * 0x10

struct Parameters{
    uint64_t Parameter0;
    uint64_t Parameter1;
    uint64_t Parameter2;
    uint64_t Parameter3;
    uint64_t Parameter4;
    uint64_t Parameter5;
}__attribute__((packed));

struct threadInfo_t{
    uint64_t SyscallStack;
    uint64_t CS;
    uint64_t SS;
    struct thread_t* Thread;
}__attribute__((packed));

struct StackInfo{
    /* stack is also use for TLS */
    uint64_t StackStart;
    uint64_t StackEndMax;
}__attribute__((packed));

struct thread_t;

struct process_t{
    /* ID infos */
    uint64_t PID;

    /* Priviledge */
    uint8_t DefaultPriviledge;

    /* Memory */
    pagetable_t SharedPaging;
    uint64_t MemoryAllocated;

    /* Childs */
    Node* Childs;
    uint64_t TID;
    uint64_t NumberOfThread;

    /* Time info */
    uint64_t CreationTime;

    /* Keyhole */
    uintptr_t Locks;
    uint64_t LockIndex;
    uint64_t LockLimit;

    /* parent */
    Node* NodeParent;
    TaskManager* TaskManagerParent;

    /* external data */
    uint64_t externalData;

    thread_t* CreateThread(uintptr_t entryPoint, uint64_t externalData);
    thread_t* CreateThread(uintptr_t entryPoint, uint8_t priviledge, uint64_t externalData);
    thread_t* DuplicateThread(thread_t* source, uint64_t externalData);
}__attribute__((packed));  

struct thread_t{
    /* ID infos */
    uint64_t TID;
    SelfData* threadData;

    /* Thread infos */
    threadInfo_t* Info;
    uintptr_t EntryPoint;

    /* Memory */
    pagetable_t Paging;
    uint64_t MemoryAllocated;

    /* Context info */
    struct ContextStack* Regs; 
    uintptr_t SIMDSaver;
    StackInfo* Stack; 
    uint64_t CoreID;
    bool IsBlock;

    /* Time info */
    uint64_t TimeAllocate;
    uint64_t CreationTime;

    /* Privledge */
    uint8_t Priviledge;
    uint8_t RingPL:3;
    uint8_t IOPL:3;

    /* Process */
    process_t* Parent;
    Node* ThreadNode;
    
    /* CIP */
    bool IsCIP;
    thread_t* TCIP;

    /* Event */
    bool IsEvent;
    uint64_t EventLock;
    struct event_data_node_t* DataNode;

    /* Schedule queue */
    bool IsInQueue;
    thread_t* Last;
    thread_t* Next;

    /* external data */
    uint64_t externalData;

    void SaveContext(struct ContextStack* Registers, uint64_t CoreID);
    void SaveContext(struct ContextStack* Registers);
    void CreateContext(struct ContextStack* Registers, uint64_t CoreID);

    void SetParameters(parameters_t* FunctionParameters);

    void SetupStack();
    void CopyStack(thread_t* source);
    bool ExtendStack(uint64_t address);
    bool ExtendStack(uint64_t address, size_t size);
    KResult ShareDataUsingStackSpace(uintptr_t data, size_t size, uint64_t* location);

    bool CIP(struct ContextStack* Registers, uint64_t CoreID, thread_t* thread, parameters_t* FunctionParameters);

    bool Launch(parameters_t* FunctionParameters);  
    bool Launch();  
    bool Pause(ContextStack* Registers, uint64_t CoreID);   
    bool Exit(ContextStack* Registers, uint64_t CoreID);
}__attribute__((packed));  

class TaskManager{
    public:
        void Scheduler(struct ContextStack* Registers, uint64_t CoreID);
        void SwitchTask(struct ContextStack* Registers, uint64_t CoreID, thread_t* task);

        void EnqueueTask(struct thread_t* task);
        void EnqueueTaskWithoutLock(struct thread_t* thread);
        void DequeueTask(struct thread_t* task);
        void DequeueTaskWithoutLock(struct thread_t* task);

        // threads
        thread_t* GetTreadWithoutLock();
        uint64_t CreateThread(thread_t** self, process_t* proc, uintptr_t entryPoint, uint64_t externalData);
        uint64_t CreateThread(thread_t** self, process_t* proc, uintptr_t entryPoint, uint8_t privilege, uint64_t externalData);
        uint64_t DuplicateThread(thread_t** self, process_t* proc, thread_t* source, uint64_t externalData);
        uint64_t ExecThread(thread_t* self, parameters_t* FunctionParameters);
        uint64_t Pause(ContextStack* Registers, uint64_t CoreID, thread_t* task); 
        uint64_t Unpause(thread_t* task); 
        uint64_t Exit(ContextStack* Registers, uint64_t CoreID, thread_t* task); 
        uint64_t ShareDataUsingStackSpace(thread_t* self, uintptr_t data, size_t size, uint64_t* location);
        // process
        uint64_t CreateProcess(process_t** key, uint8_t priviledge, uint64_t externalData);

        void CreateIddleTask();   

        void InitScheduler(uint8_t NumberOfCores, uintptr_t IddleTaskFunction); 
        void EnabledScheduler(uint64_t CoreID);
        thread_t* GetCurrentThread(uint64_t CoreID);

        bool IsSchedulerEnable[MAX_PROCESSORS];
        uint64_t TimeByCore[MAX_PROCESSORS];
        thread_t* ThreadExecutePerCore[MAX_PROCESSORS];

        bool TaskManagerInit;  
        uint64_t MutexScheduler;  

        uint64_t NumberProcessTotal = 0;
        uint64_t NumberOfCPU = 0;
        uint64_t CurrentTaskExecute = 0;
        uint64_t IddleTaskNumber = 0;
        uintptr_t IddleTaskPointer = 0;
        uint64_t PID = 0;

        thread_t* FirstNode;
        thread_t* LastNode;

        Node* ProcessList = NULL;
        //iddle
        process_t* IddleProc = NULL;
        thread_t* IdleNode[MAX_PROCESSORS];    
        Node* GlobalProcessNode;

        uint64_t lockglobalAddressForStackSpaceSharing;
        uintptr_t globalAddressForStackSpaceSharing;
};



extern TaskManager* globalTaskManager;