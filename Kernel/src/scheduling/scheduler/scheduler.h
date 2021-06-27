#pragma once
#include <stdint.h>
#include "../../userspace/userspace/userspace.h"
#include "../../paging/pageTableManager.h"
#include "../../hardware/apic/apic.h"
#include "../../memory/heap.h"
#include "../../lib/limits.h"
#include "../../lib/stdio.h"
#include "../../tss/tss.h"
#include "../../graphics.h"

typedef struct ContextStack {
    void* rax; void* rbx; void* rcx; void* rdx; void* KernelRsp; void* rsi; void* rdi; void* rbp; //push in asm

    void* r8; void* r9; void* r10; void* r11; void* r12; void* r13; void* r14; void* r15; //push in asm

    void* rip; void* cs; void* rflags; void* rsp; void* ss; //push by cpu with an interrupt
}__attribute__((packed));

struct TaskContext{
    void* EntryPoint;
    void* Stack;
    ContextStack Regs; 
    uint64_t ID;
    bool IsIddle;
    void* parent; // if task is thread    
};

struct TaskNode{
	TaskContext Content;
	TaskNode* Last;
	TaskNode* Next;
} __attribute__((packed));

class TaskManager{
    public:
        void Scheduler(struct InterruptStack* Registers, uint8_t CoreID);
        TaskNode* AddTask(void* EntryPoint, size_t Size, bool IsIddle);    
        TaskNode* CreatDefaultTask();     
        void DeleteTask(TaskNode* task);  
        void InitScheduler(uint8_t NumberOfCores);
        void EnabledScheduler(uint8_t CoreID);
        TaskNode* GetCurrentTask(uint8_t CoreID);
        bool CoreInUserSpace[MAX_PROCESSORS];
        size_t IddleTaskNumber = 0;

    private:  
        bool TaskManagerInit;  
        uint64_t CurrentTaskExecute;
        size_t NumTaskTotal = 0;
        TaskNode* MainNode = NULL;
        TaskNode* LastNode = NULL;
        TaskNode* FirstNode = NULL;
        TaskNode* MainNodeScheduler = NULL;

        //iddle
        TaskNode* IdleNode[MAX_PROCESSORS];         
        
        TaskNode* NodeExecutePerCore[MAX_PROCESSORS];
};



extern TaskManager globalTaskManager;