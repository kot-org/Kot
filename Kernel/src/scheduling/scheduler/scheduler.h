#pragma once
#include "../../lib/types.h"
#include "../../arch/x86-64/userspace/userspace/userspace.h"
#include "../../memory/paging/pageTableManager.h"
#include "../../memory/heap/heap.h"
#include "../../lib/limits.h"
#include "../../arch/x86-64/tss/tss.h"

struct ContextStack;
struct TaskQueuNode;
struct Task;
class TaskManager;

#define MaxNameTask 256

struct ContextStack{
    void* rax; void* rbx; void* rcx; void* rdx; void* rsi; void* rdi; void* rbp; //push in asm

    void* r8; void* r9; void* r10; void* r11; void* r12; void* r13; void* r14; void* r15; //push in asm

    void* rip; void* cs; rflags_t rflags; void* rsp; void* ss; //push by cpu with an interrupt
}__attribute__((packed));

struct Parameters{
    uint64_t Parameter0;
    uint64_t Parameter1;
    uint64_t Parameter2;
    uint64_t Parameter3;
    uint64_t Parameter4;
    uint64_t Parameter5;
}__attribute__((packed));

struct StackInfo{
    uint64_t StackStart;
    uint64_t StackEnd;
    uint64_t StackEndMax;
}__attribute__((packed));

struct thread_t;

struct process_t{
    /* ID infos */
    uint64_t PID;

    /* Priviledge */
    uint8_t DefaultPriviledge:3;

    /* Memory */
    PageTableManager* SharedPaging;

    /* Childs */
    Node* Childs;
    uint64_t TID;
    uint64_t NumberOfThread;

    /* Time info */
    uint64_t CreationTime;

    /* parent */
    Node* NodeParent;
    TaskManager* TaskManagerParent;

    /* external data */
    void* externalData;

    thread_t* CreatThread(uint64_t entryPoint, void* externalData);
    thread_t* CreatThread(uint64_t entryPoint, uint8_t priviledge, void* externalData);
    thread_t* DuplicateThread(thread_t* source);
}__attribute__((packed));  

struct thread_t{
    /* ID infos */
    uint64_t TID;

    /* Thread infos */
    void* EntryPoint;

    /* Memory */
    PageTableManager* Paging;
    uint64_t MemoryAllocated;

    /* Context info */
    ContextStack* Regs; 
    StackInfo* Stack;
    uint8_t CoreID;
    bool IsBlock;

    /* Time info */
    uint64_t TimeAllocate;
    uint64_t CreationTime;

    /* Privledge */
    uint8_t RingPL:3;
    uint8_t IOPL:3;
    

    /* Process */
    process_t* Parent;
    Node* ThreadNode;
    
    /* Fork */
    bool IsForked;
    thread_t* ForkedThread;

    /* Schedule queue */
    bool IsInQueue;
    thread_t* Last;
    thread_t* Next;

    /* external data */
    void* externalData;

    void SaveContext(struct InterruptStack* Registers, uint8_t CoreID);
    void CreatContext(struct InterruptStack* Registers, uint8_t CoreID);

    void SetParameters(Parameters* FunctionParameters);

    void SetupStack();
    void CopyStack(thread_t* source);
    bool ExtendStack(uint64_t address);

    bool Fork(struct InterruptStack* Registers, uint8_t CoreID, thread_t* thread, Parameters* FunctionParameters);
    bool Fork(struct InterruptStack* Registers, uint8_t CoreID, thread_t* thread);

    bool Launch(Parameters* FunctionParameters);  
    bool Launch();  
    bool Pause(InterruptStack* Registers, uint8_t CoreID);  
    bool Exit(InterruptStack* Registers, uint8_t CoreID);  

    bool SetIOPriviledge(ContextStack* Registers, uint8_t IOPL);
}__attribute__((packed));  

class TaskManager{
    public:
        void Scheduler(struct InterruptStack* Registers, uint8_t CoreID);
        void SwitchTask(struct InterruptStack* Registers, uint8_t CoreID, thread_t* task);

        void EnqueueTask(struct thread_t* task);
        void DequeueTask(struct thread_t* task);
        void DequeueTaskWithoutLock(struct thread_t* task);

        // threads
        thread_t* GetTread();
        bool ExecThread(thread_t* self, Parameters* FunctionParameters);

        // process
        process_t* CreatProcess(uint8_t priviledge, void* externalData);

        void CreatIddleTask();   

        void InitScheduler(uint8_t NumberOfCores); 
        void EnabledScheduler(uint8_t CoreID);
        thread_t* GetCurrentThread(uint8_t CoreID);

        bool IsSchedulerEnable[MAX_PROCESSORS];
        uint64_t TimeByCore[MAX_PROCESSORS];
        thread_t* ThreadExecutePerCore[MAX_PROCESSORS];

        bool TaskManagerInit;  

        uint64_t NumberProcessTotal = 0;
        uint64_t CurrentTaskExecute = 0;
        uint64_t IddleTaskNumber = 0;
        uint64_t PID = 0;

        thread_t* FirstNode;
        thread_t* LastNode;

        Node* ProcessList = NULL;
        //iddle
        process_t* IddleProc = NULL;
        thread_t* IdleNode[MAX_PROCESSORS];    
        Node* GlobalProcessNode;
};



extern TaskManager* globalTaskManager;