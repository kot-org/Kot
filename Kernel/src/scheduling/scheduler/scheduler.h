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

#define MaxNameTask 256

struct ContextStack{
    void* rax; void* rbx; void* rcx; void* rdx; void* rsi; void* rdi; void* rbp; //push in asm

    void* r8; void* r9; void* r10; void* r11; void* r12; void* r13; void* r14; void* r15; //push in asm

    void* rip; void* cs; void* rflags; void* rsp; void* ss; //push by cpu with an interrupt
}__attribute__((packed));

struct Parameters{
    uint64_t Parameter0;
    uint64_t Parameter1;
    uint64_t Parameter2;
    uint64_t Parameter3;
    uint64_t Parameter4;
    uint64_t Parameter5;
}__attribute__((packed));

struct TaskContext{
    //task memory
    PageTableManager paging;
    UserHeap::Heap* heap;
    void* Stack;
    bool IsKernelStack;

    //task info
    void* EntryPoint;
    ContextStack* Regs; 
    uint64_t PID;
    bool IsIddle;
    bool IsRunning;
    bool IsPaused;
    bool IsTaskInTask;
    uint64_t TimeUsed;
    bool IsThread;
    uint8_t Priviledge:3;
    char Name[MaxNameTask];

    //other data
    TaskNode* TaskToLaunchWhenExit;

    //parent 
    TaskContext* ThreadParent; // if task is thread  
    TaskNode* NodeParent;
    TaskManager* TaskManagerParent;
    uint8_t CoreID;

    //child
    //function
    void CreatThread();  
    void Launch(void* EntryPoint);
    void Launch(void* EntryPoint, Parameters* FunctionParameters);
    void Exit();
    void* ExitTaskInTask(struct InterruptStack* Registers, uint8_t CoreID, void* returnValue);
}__attribute__((packed));

struct TaskNode{
	TaskContext Content;
	TaskNode* Last;
	TaskNode* Next;
}__attribute__((packed));

struct DeviceTaskAdressStruct{
	uint8_t type:3;
    uint16_t L1:10;
    uint16_t L2:10;
    uint16_t L3:10;
    uint16_t FunctionID:9;
}__attribute__((packed));

struct DeviceTaskData{
    DeviceTaskAdressStruct* DeviceTaskAdress;
    TaskContext* task; 
    TaskNode* parent;   
}__attribute__((packed));

struct DeviceTaskTableEntry { 
    void* entries[1024];
}__attribute__((aligned(0x1000))); 

struct DeviceTaskTableStruct{
    DeviceTaskData* GetDeviceTaskData(DeviceTaskAdressStruct* DeviceAdress);
    void SetDeviceTaskData(DeviceTaskAdressStruct* DeviceAdress, DeviceTaskData* deviceTaskData);

    DeviceTaskTableEntry* DeviceTaskTableKernel;
    DeviceTaskTableEntry* DeviceTaskTableDriver;
    DeviceTaskTableEntry* DeviceTaskTableDevice;
    DeviceTaskTableEntry* DeviceTaskTableApp;
}__attribute__((packed));

class TaskManager{
    public:
        void Scheduler(struct InterruptStack* Registers, uint8_t CoreID);
        uint64_t ExecuteSubTask(struct InterruptStack* Registers, uint8_t CoreID, DeviceTaskAdressStruct* DeviceAdress, Parameters* FunctionParameters);
        void CreatSubTask(TaskNode* parent, void* EntryPoint, DeviceTaskAdressStruct* DeviceAdress);
        TaskNode* AddTask(bool IsIddle, bool IsLinked, int ring, char* name);    
        TaskNode* NewNode(TaskNode* node);
        TaskNode* CreatDefaultTask(bool IsLinked);     
        void DeleteTask(TaskNode* task); 
        void InitScheduler(uint8_t NumberOfCores); 
        void EnabledScheduler(uint8_t CoreID);
        TaskNode* GetCurrentTask(uint8_t CoreID);
        bool CoreInUserSpace[MAX_PROCESSORS];
        uint64_t TimeByCore[MAX_PROCESSORS];
        TaskNode* NodeExecutePerCore[MAX_PROCESSORS];
        size_t NumTaskTotal = 0;

    private:  
        bool TaskManagerInit;  
        uint64_t CurrentTaskExecute;
        size_t IddleTaskNumber = 0;
        size_t IDTask = 0;
        TaskNode* LastNode = NULL;
        TaskNode* FirstNode = NULL;
        TaskNode* MainNodeScheduler = NULL;

        //iddle
        TaskNode* IdleNode[MAX_PROCESSORS];    
        DeviceTaskTableStruct DeviceTaskTable;
};



extern TaskManager* globalTaskManager;