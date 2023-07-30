#pragma once
#include <mm/mm.h>
#include <lib/pid.h>
#include <arch/arch.h>
#include <lib/types.h>
#include <heap/heap.h>
#include <lib/limits.h>
#include <lib/vector.h>
#include <event/event.h>
#include <abi-bits/signal.h>
#include <keyhole/keyhole.h>

struct TaskQueuNode;
struct Task;
class TaskManager;

#define SELF_DATA_START_ADDRESS vmm_MapAddress(0xfe, 0, 0, 0)  
#define SELF_DATA_END_ADDRESS vmm_MapAddress(0xff, 0, 0, 0)  
#define STACK_TOP vmm_MapAddress(0x100, 0, 0, 0)
#define STACK_BOTTOM SELF_DATA_END_ADDRESS 
#define LOCK_ADDRESS vmm_MapAddress(0xfd, 0, 0, 0) 
#define FREE_MEMORY_SPACE_ADDRESS LOCK_ADDRESS
#define APP_STACK_MAX_SIZE vmm_MapAddress(0, 0, 1, 0)
#define USERSPACE_TOP_ADDRESS vmm_MapAddress(0x100, 0, 0, 0)
#define DEFAULT_FLAGS_KEY 0xff


inline uint64_t StackAlignToJmp(uint64_t Stack){
    // Have to be 16 byte aligned before call so if we jump we have to be non 16 bytes aligned but 8 bytes aligned
    if((Stack & 0xfffffffffffffff0) != 0 && (Stack & 0xfffffffffffffff0) != 8){
        Stack = (Stack & 0xfffffffffffffff0) - 8;
    }else if(Stack & 0xfffffffffffffff0 != 8){
        Stack -= 8;
    }
    return Stack;
}

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
    uint64_t LastStackUsed;
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
    void* Data;
    uint8_t ParameterPosition;
}__attribute__((packed));


struct kthread_t;

struct kprocess_t{
    /* ID infos */
    pid_t PID;
    pid_t PPID;
    pid_t PIDKey;

    bool HaveForkPaging;
    uint64_t ListIndex;

    struct kprocess_t* Parent;

    /* Priviledge */
    enum Priviledge DefaultPriviledge;

    /* Memory */
    struct MemoryHandler_t* MemoryManager;
    pagetable_t SharedPaging;
    uint64_t MemoryAllocated;

    /* Childs */
    struct Node* Childs;
    uint64_t TID;
    uint64_t NumberOfthread;

    /* Time info */
    uint64_t CreationTime;

    /* Keyhole */
    void* Locks;
    uint64_t LockIndex;
    uint64_t LockLimit;
    key_t ProcessKey;

    /* Parent */
    struct Node* NodeParent;
    TaskManager* TaskManagerParent;
    locker_t CreateThreadLocker;

    /* Process Creator Info */
    uint64_t PID_PCI;
    uint64_t PPID_PCI;
    uint64_t TID_PCI;
    uint64_t ExternalData_P_PCI;
    uint64_t Priviledge_PCI;

    /* External data */
    uint64_t ExternalData_P;

    /* Stack thread */
    locker_t StackIteratorLock;
    uintptr_t StackIterator;

    /* WaitPID */
    locker_t WaitPIDLock;
    class KStack* WaitPIDList;
    uint64_t WaitPIDCount;

    /* Process Child */
    uint64_t ProcessChildCount;

    kthread_t* Createthread(void* entryPoint, uint64_t externalData);
    kthread_t* Createthread(void* entryPoint, enum Priviledge priviledge, uint64_t externalData);
    kthread_t* Duplicatethread(kthread_t* source);
    KResult Fork(struct ContextStack* Registers, kthread_t* Caller, kprocess_t** Child, kthread_t** ChildThread);
}__attribute__((packed));  

struct kthread_t{
    /* ID infos */
    uint64_t TID;
    SelfData* threadData;

    /* KernelInternalStack */
    void* KernelInternalStack;
    void* FSBase;

    /* Thread infos */
    threadInfo_t* Info;
    void* EntryPoint;

    /* Memory */
    pagetable_t Paging;
    uint64_t MemoryAllocated;

    /* Context info */
    struct ContextStack* Regs; 
    void* SIMDSaver;
    StackInfo* Stack; 
    uint64_t CoreID;
    bool IsBlock;
    bool IsClose;
    bool IsPause;
    bool IsFork;
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
    struct Node* ThreadNode;
    
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

    /* PIDWait */
    int PIDWaitStatus;
    pid_t PIDWaitPIDChild;

    /* Signals */
    sigset_t SignalMask;

    void SaveContext(struct ContextStack* Registers, uint64_t CoreID);
    void SaveContext(struct ContextStack* Registers);
    void CreateContext(struct ContextStack* Registers, uint64_t CoreID);
    void ResetContext(ContextStack* Registers);

    void SetParameters(arguments_t* FunctionParameters);

    bool PageFaultHandler(bool IsWriting, uint64_t Address);

    KResult SetupStack();
    bool ExtendStack(uint64_t address);
    bool ExtendStack(uint64_t address, size64_t size);
    KResult ShareDataUsingStackSpace(void* data, size64_t size, void** location);

    bool Launch(arguments_t* FunctionParameters);  
    bool Launch_WL(arguments_t* FunctionParameters);  
    bool Launch();
    bool Launch_WL();  
    bool Pause(ContextStack* Registers, bool force);   
    bool Pause_WL(ContextStack* Registers, bool force);   
    KResult Close(ContextStack* Registers, uint64_t ReturnValue);
    KResult CloseQueu(uint64_t ReturnValue);

    kthread_t* ForkThread(ContextStack* Registers, kprocess_t* Child);
}__attribute__((packed));  

class TaskManager{
    public:
        void Scheduler(struct ContextStack* Registers, uint64_t CoreID);
        void DestroySelf(struct ContextStack* Registers, uint64_t CoreID);
        void PauseSelf(ContextStack* Registers, uint64_t CoreID);
        void SwitchTask(struct ContextStack* Registers, uint64_t CoreID, kthread_t* task);

        void EnqueueTask(struct kthread_t* task);
        void EnqueueTask_WL(struct kthread_t* thread);
        void DequeueTask(struct kthread_t* task);
        void DequeueTask_WL(struct kthread_t* task);

        // threads
        kthread_t* GetTread_WL();
        KResult Createthread(kthread_t** self, kprocess_t* proc, void* entryPoint, uint64_t externalData);
        KResult Createthread(kthread_t** self, kprocess_t* proc, void* entryPoint, enum Priviledge priviledge, uint64_t externalData);
        KResult Duplicatethread(kthread_t** self, kprocess_t* proc, kthread_t* source);
        KResult Execthread(kthread_t* Caller, kthread_t* Self, execution_type_t Type, arguments_t* FunctionParameters, ThreadShareData_t* Data, ContextStack* Registers);
        KResult Unpause(kthread_t* task); 
        KResult Unpause_WL(kthread_t* task); 
        KResult Exit(ContextStack* Registers, kthread_t* task, uint64_t ReturnValue); 
        // process
        KResult CreateProcessWithoutPaging(kprocess_t** key, enum Priviledge priviledge, uint64_t externalData);
        KResult CreateProcessWithoutPaging(kthread_t* caller, kprocess_t** key, enum Priviledge priviledge, uint64_t externalData);
        KResult CreateProcess(kprocess_t** key, enum Priviledge priviledge, uint64_t externalData);
        KResult CreateProcess(kthread_t* caller, kprocess_t** key, enum Priviledge priviledge, uint64_t externalData);

        void CreateIddleTask();   

        void InitScheduler(uint8_t NumberOfCores, void* IddleTaskFunction); 
        void EnabledScheduler(uint64_t CoreID);
        kthread_t* GetCurrentthread(uint64_t CoreID);

        void AcquireScheduler();
        void ReleaseScheduler();
        
        void PauseSelf();

        KResult AddProcessList(kprocess_t* process);
        KResult RemoveProcessList(kprocess_t* process);
        kprocess_t* GetProcessList(pid_t pid);

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
        void* IddleTaskPointer = 0;
        pid_t PID = 0;

        kthread_t* FirstNode;
        kthread_t* LastNode;

        kot_vector_t* ProcessList;

        //iddle
        kprocess_t* KernelProc = NULL; 
        kthread_t* IdleNode[MAX_PROCESSORS];

};

void SetParameters(ContextStack* Registers, arguments_t* FunctionParameters);

extern TaskManager* globalTaskManager;