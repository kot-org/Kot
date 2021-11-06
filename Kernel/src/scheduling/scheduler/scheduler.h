#pragma once
#include "../../lib/types.h"
#include "../../arch/x86-64/userspace/userspace/userspace.h"
#include "../../memory/paging/pageTableManager.h"
#include "../../hardware/apic/apic.h"
#include "../../memory/heap/heap.h"
#include "../../lib/limits.h"
#include "../../lib/stdio.h"
#include "../../arch/x86-64/tss/tss.h"
#include "../../drivers/graphics/graphics.h"
#include "../../memory/UserHeap/heap.h"

struct ContextStack {
    void* rax; void* rbx; void* rcx; void* rdx; void* rsi; void* rdi; void* rbp; //push in asm

    void* r8; void* r9; void* r10; void* r11; void* r12; void* r13; void* r14; void* r15; //push in asm

    void* rip; void* cs; void* rflags; void* rsp; void* ss; //push by cpu with an interrupt
}__attribute__((packed));

struct TaskContext{
    PageTableManager paging;
    UserHeap::Heap* heap;
    void* EntryPoint;
    void* Stack;
    ContextStack Regs; 
    uint64_t ID;
    bool IsIddle;
    bool IsRunning;
    bool IsPaused;
    void* parent; // if task is thread  

    void CreatThread();  
    void Launch(void* EntryPoint);
}__attribute__((packed));

struct TaskNode{
	TaskContext Content;
	TaskNode* Last;
	TaskNode* Next;
}__attribute__((packed));

class TaskManager{
    public:
        void Scheduler(struct InterruptStack* Registers, uint8_t CoreID);
        TaskNode* AddTask(bool IsIddle, bool IsLinked, int ring);    
        TaskNode* NewNode(TaskNode* node);
        TaskNode* CreatDefaultTask(bool IsLinked);     
        void DeleteTask(TaskNode* task); 
        void InitScheduler(uint8_t NumberOfCores); 
        void EnabledScheduler(uint8_t CoreID);
        TaskNode* GetCurrentTask(uint8_t CoreID);
        bool CoreInUserSpace[MAX_PROCESSORS];
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
        

};



extern TaskManager globalTaskManager;