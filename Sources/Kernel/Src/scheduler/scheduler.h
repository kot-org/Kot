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

struct threadInfo_t{
    uint64_t SyscallStack;
    uint64_t CS;
    uint64_t SS;
    struct kthread_t* thread;
}__attribute__((packed));

struct StackInfo{
    /* stack is also use for TLS */
    uint64_t StackStart;
    uint64_t StackEndMax;
}__attribute__((packed));

struct ThreadQueu_t{
    locker_t Lock;
    uint64_t TasksInQueu; /* The queued task includes the running task */
    struct ThreadQueuData_t* CurrentData;
    struct ThreadQueuData_t* LastData;
    KResult SetThreadInQueu(kthread_t* Caller, kthread_t* Self, arguments_t* FunctionParameters, bool IsAwaitTask, struct ThreadShareData_t* Data);
    KResult SetThreadInQueu_NSU(kthread_t* Caller, kthread_t* Self, arguments_t* FunctionParameters, bool IsAwaitTask, struct ThreadShareData_t* Data);
    KResult ExecuteThreadInQueu();
    KResult NextThreadInQueu_WL();
}__attribute__((packed));

struct ThreadQueuData_t{
    kthread_t* Task;
    kthread_t* Caller;
    arguments_t Parameters;
    struct ThreadShareData_t* Data;
    bool IsAwaitTask;
    kthread_t* AwaitTask;
    ThreadQueuData_t* Next;
}__attribute__((packed));

struct ThreadShareData_t{
    size64_t Size;
    uintptr_t Data;
    uint8_t ParameterPosition;
}__attribute__((packed));


struct kthread_t;

struct kprocess_t{
    /* ID infos */
    uint64_t PID;

    /* Priviledge */
    enum Priviledge DefaultPriviledge;

    /* Memory */
    pagetable_t SharedPaging;
    uint64_t MemoryAllocated;

    /* Childs */
    Node* Childs;
    uint64_t TID;
    uint64_t NumberOfthread;

    /* Time info */
    uint64_t CreationTime;

    /* Keyhole */
    uintptr_t Locks;
    uint64_t LockIndex;
    uint64_t LockLimit;
    key_t ProcessKey;

    /* Parent */
    Node* NodeParent;
    TaskManager* TaskManagerParent;
    locker_t CreateThreadLocker;

    /* Process Creator Info */
    uint64_t PID_PCI;
    uint64_t TID_PCI;
    uint64_t ExternalData_P_PCI;
    uint64_t Priviledge_PCI;

    /* External data */
    uint64_t ExternalData_P;

    kthread_t* Createthread(uintptr_t entryPoint, uint64_t externalData);
    kthread_t* Createthread(uintptr_t entryPoint, enum Priviledge priviledge, uint64_t externalData);
    kthread_t* Duplicatethread(kthread_t* source);
}__attribute__((packed));  

struct kthread_t{
    /* ID infos */
    uint64_t TID;
    SelfData* threadData;

    /* KernelInternalStack */
    uintptr_t KernelInternalStack;

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
    bool IsClose;
    bool IsPause;
    uint64_t UnpauseOverflowCounter;

    /* Time info */
    uint64_t TimeAllocate;
    uint64_t CreationTime;

    /* Privledge */
    enum Priviledge Priviledge;
    uint8_t RingPL:3;
    uint8_t IOPL:3;

    /* Process */
    kprocess_t* Parent;
    Node* ThreadNode;
    
    /* Queu */
    ThreadQueu_t* Queu;

    /* Event */
    bool IsEvent;
    locker_t EventLock;
    struct event_data_node_t* EventDataNode;

    /* Schedule queue */
    bool IsInQueue;
    kthread_t* Last;
    kthread_t* Next;

    /* External data */
    uint64_t ExternalData_T;

    void SaveContext(struct ContextStack* Registers, uint64_t CoreID);
    void SaveContext(struct ContextStack* Registers);
    void CreateContext(struct ContextStack* Registers, uint64_t CoreID);
    void ResetContext(ContextStack* Registers);

    void SetParameters(arguments_t* FunctionParameters);

    void SetupStack();
    void CopyStack(kthread_t* source);
    bool ExtendStack(uint64_t address);
    bool ExtendStack(uint64_t address, size64_t size);
    KResult ShareDataUsingStackSpace(uintptr_t data, size64_t size, uintptr_t* location);

    bool Launch(arguments_t* FunctionParameters);  
    bool Launch_WL(arguments_t* FunctionParameters);  
    bool Launch();
    bool Launch_WL();  
    bool Pause(ContextStack* Registers, bool force);   
    bool Pause_WL(ContextStack* Registers, bool force);   
    KResult Close(ContextStack* Registers, uint64_t ReturnValue);
    KResult CloseQueu(uint64_t ReturnValue);
}__attribute__((packed));  

class TaskManager{
    public:
        void Scheduler(struct ContextStack* Registers, uint64_t CoreID);
        void DestroySelf(struct ContextStack* Registers, uint64_t CoreID);
        void SwitchTask(struct ContextStack* Registers, uint64_t CoreID, kthread_t* task);

        void EnqueueTask(struct kthread_t* task);
        void EnqueueTask_WL(struct kthread_t* thread);
        void DequeueTask(struct kthread_t* task);
        void DequeueTask_WL(struct kthread_t* task);

        // threads
        kthread_t* GetTread_WL();
        uint64_t Createthread(kthread_t** self, kprocess_t* proc, uintptr_t entryPoint, uint64_t externalData);
        uint64_t Createthread(kthread_t** self, kprocess_t* proc, uintptr_t entryPoint, enum Priviledge priviledge, uint64_t externalData);
        uint64_t Duplicatethread(kthread_t** self, kprocess_t* proc, kthread_t* source);
        KResult Execthread(kthread_t* Caller, kthread_t* Self, enum ExecutionType Type, arguments_t* FunctionParameters, ThreadShareData_t* Data, ContextStack* Registers);
        uint64_t Unpause(kthread_t* task); 
        uint64_t Unpause_WL(kthread_t* task); 
        uint64_t Exit(ContextStack* Registers, kthread_t* task, uint64_t ReturnValue); 
        // process
        uint64_t CreateProcess(kprocess_t** key, enum Priviledge priviledge, uint64_t externalData);
        uint64_t CreateProcess(kthread_t* caller, kprocess_t** key, enum Priviledge priviledge, uint64_t externalData);

        void CreateIddleTask();   

        void InitScheduler(uint8_t NumberOfCores, uintptr_t IddleTaskFunction); 
        void EnabledScheduler(uint64_t CoreID);
        kthread_t* GetCurrentthread(uint64_t CoreID);

        void AcquireScheduler();
        void ReleaseScheduler();

        bool IsSchedulerEnable[MAX_PROCESSORS];
        uint64_t TimeByCore[MAX_PROCESSORS];
        kthread_t* ThreadExecutePerCore[MAX_PROCESSORS];

        bool TaskManagerInit;

        locker_t SchedulerLock;  
        locker_t CreateProcessLock;  

        uint64_t NumberProcessTotal = 0;
        uint64_t NumberOfCPU = 0;
        uint64_t CurrentTaskExecute = 0;
        uint64_t IddleTaskNumber = 0;
        uintptr_t IddleTaskPointer = 0;
        uint64_t PID = 0;

        kthread_t* FirstNode;
        kthread_t* LastNode;

        Node* ProcessList = NULL;
        //iddle
        kprocess_t* KernelProc = NULL; 
        kthread_t* IdleNode[MAX_PROCESSORS];    
        Node* GlobalProcessNode;
};

void SetParameters(ContextStack* Registers, arguments_t* FunctionParameters);

extern TaskManager* globalTaskManager;