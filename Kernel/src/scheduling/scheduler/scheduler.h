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
struct TaskQueuNode;
struct Task;
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

struct DeviceTaskAdressStruct{
	uint8_t type:3;
    uint16_t L1:10;
    uint16_t L2:10;
    uint16_t L3:10;
    uint16_t FunctionID:10;
}__attribute__((packed));

struct DeviceTaskData{
    Task* task; 
    Task* parent;   
}__attribute__((packed));

struct DeviceTaskTableEntry { 
    void* entries[1024];
}__attribute__((packed)); 

struct Task{
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
    bool IsPaused;
    bool IsTaskInTask;
    bool InterruptTask;
    uint64_t TimeAllocate;
    uint64_t CreationTime;
    bool IsThread;
    uint8_t Priviledge:3;
    char Name[MaxNameTask];
    uint64_t MemoryAllocated;

    //other data
    Task* TaskToLaunchWhenExit;

    //parent 
    Task* Parent;
    Node* NodeParent;
    TaskManager* TaskManagerParent;
    uint8_t CoreID;

    TaskQueuNode* TaskQueueParent; //only if task is in qeue and not run

    //Queue
    bool IsInQueue;
    Task* Last;
    Task* Next;

    //child
    //function
    void CreatThread();  
    void Launch(void* EntryPoint);
    void Launch(void* EntryPoint, Parameters* FunctionParameters);
    void Exit(uint8_t CoreID);
    void Pause(uint8_t CoreID, struct InterruptStack* Registers);
    void Unpause();
    void ExitIRQ();
    void* ExitTaskInTask(struct InterruptStack* Registers, uint8_t CoreID, void* returnValue);
    uint64_t ExecuteSubTask(InterruptStack* Registers, uint8_t CoreID, DeviceTaskAdressStruct* DeviceAdress, Parameters* FunctionParameters);
}__attribute__((packed));

struct TaskQueuNode{
	Task* TaskData;
	TaskQueuNode* Last;
	TaskQueuNode* Next;
}__attribute__((packed));


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
        void SwitchTask(struct InterruptStack* Registers, uint8_t CoreID, Task* task);
        Task* DuplicateTask(Task* parent);

        void EnqueueTask(Task* task);
        void DequeueTask(Task* task);
        void DequeueTaskWithoutLock(Task* task);
        Task* GetTask();

        void CreatSubTask(Task* parent, void* EntryPoint, DeviceTaskAdressStruct* DeviceAdress);
        Task* AddTask(uint8_t priviledge, char* name);    
        Task* CreatDefaultTask();     
        void InitScheduler(uint8_t NumberOfCores); 
        void EnabledScheduler(uint8_t CoreID);
        Task* GetCurrentTask(uint8_t CoreID);
        bool IsSchedulerEnable[MAX_PROCESSORS];
        uint64_t TimeByCore[MAX_PROCESSORS];
        Task* NodeExecutePerCore[MAX_PROCESSORS];

        bool TaskManagerInit;  

        uint64_t NumberTaskTotal = 0;
        uint64_t CurrentTaskExecute = 0;
        uint64_t IddleTaskNumber = 0;
        uint64_t IDTask = 0;

        Task* FirstNode;
        Task* LastNode;

        Node* AllTasks = NULL;
        //iddle
        Task* IdleNode[MAX_PROCESSORS];    
        DeviceTaskTableStruct DeviceTaskTable;
};



extern TaskManager* globalTaskManager;