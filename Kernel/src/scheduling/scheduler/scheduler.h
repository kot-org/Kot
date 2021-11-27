#pragma once
#include "../../lib/types.h"
#include "../../arch/x86-64/userspace/userspace/userspace.h"
#include "../../arch/x86-64/tss/tss.h"
#include "../../memory/paging/pageTableManager.h"
#include "../../hardware/apic/apic.h"
#include "../../memory/heap/heap.h"
#include "../../lib/limits.h"
#include "../../lib/stdio.h"
#include "../../arch/x86-64/tss/tss.h"
#include "../../drivers/graphics/graphics.h"
#include "../../memory/UserHeap/heap.h"

struct ContextStack;
struct TaskContext;
struct TaskNode;
class TaskManager;

struct ContextStack{
    void* rax; void* rbx; void* rcx; void* rdx; void* rsi; void* rdi; void* rbp; //push in asm

    void* r8; void* r9; void* r10; void* r11; void* r12; void* r13; void* r14; void* r15; //push in asm

    void* rip; void* cs; void* rflags; void* rsp; void* ss; //push by cpu with an interrupt
}__attribute__((packed));

struct TaskContext{
    //task memory
    PageTableManager paging;
    UserHeap::Heap* heap;
    void* Stack;

    //task info
    void* EntryPoint;
    ContextStack* Regs; 
    uint64_t PID;
    bool IsIddle;
    bool IsRunning;
    bool IsPaused;
    uint64_t TimeUsed;
    bool IsThread;

    //other data
    TaskContext* TaskToLaunchWhenExit;

    //parent 
    TaskContext* ThreadParent; // if task is thread  
    TaskNode* NodeParent;
    TaskManager* TaskManagerParent;
    uint8_t CoreID;

    //child
    //function
    void CreatThread();  
    void Launch(void* EntryPoint);
    void Exit();
}__attribute__((packed));

struct TaskNode{
	TaskContext Content;
	TaskNode* Last;
	TaskNode* Next;
}__attribute__((packed));

class TaskManager{
    public:
        void Scheduler(struct InterruptStack* Registers, uint8_t CoreID);
        void SwitchTask(InterruptStack* Registers, uint8_t CoreID);
        void SwitchTask(InterruptStack* Registers, uint8_t CoreID, GUID* APIGUID, uint64_t SpecialEntryPoint, bool IsSpecialEntryPoint);
        TaskNode* AddTask(bool IsIddle, bool IsLinked, int ring);    
        TaskNode* NewNode(TaskNode* node);
        TaskNode* CreatDefaultTask(bool IsLinked);     
        void DeleteTask(TaskNode* task); 
        void InitScheduler(uint8_t NumberOfCores); 
        void EnabledScheduler(uint8_t CoreID);
        TaskNode* GetCurrentTask(uint8_t CoreID);
        bool CoreInUserSpace[MAX_PROCESSORS];
        uint64_t TimeByCore[MAX_PROCESSORS];
        TaskNode* NodeExecutePerCore[MAX_PROCESSORS];

    private:  
        bool TaskManagerInit;  
        uint64_t CurrentTaskExecute;
        size_t IddleTaskNumber = 0;
        size_t NumTaskTotal = 0;
        size_t IDTask = 0;
        TaskNode* LastNode = NULL;
        TaskNode* FirstNode = NULL;
        TaskNode* MainNodeScheduler = NULL;

        //iddle
        TaskNode* IdleNode[MAX_PROCESSORS];    
        Node* APINode;
};



extern TaskManager* globalTaskManager;