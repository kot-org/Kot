#include "scheduler.h"


Schedule schedule;

void* Schedule::Main(void* stack){

    void* rip = 0;
    InterruptStack* stackRegs = (InterruptStack*)stack;
    if(IsEnabled){
 
        Task* LatsTask = Tasks[PIDexec];

        LatsTask->registers.rax = GetRax();  
        LatsTask->registers.rbx = GetRbx();        
        LatsTask->registers.rcx = GetRcx();        
        LatsTask->registers.rdx = GetRdx();
        LatsTask->registers.rsi = GetRsi();
        LatsTask->registers.rdi = GetRdi();
        LatsTask->registers.rbp = GetRbp();
        LatsTask->registers.rsp = GetRsp();
        LatsTask->registers.r8 = GetR8();
        LatsTask->registers.r9 = GetR9();
        LatsTask->registers.r10 = GetR10();
        LatsTask->registers.r11 = GetR11();
        LatsTask->registers.r12 = GetR12();
        LatsTask->registers.r13 = GetR13();
        LatsTask->registers.r14 = GetR14();
        LatsTask->registers.r15 = GetR15();
        LatsTask->registers.rip = (void*)stackRegs->rip;

        PIDexec++;

        if(PIDexec >= PID){
            PIDexec = 0;
        }


        Task* task = Tasks[PIDexec];
        if(task->IsInit){
            //restore registers
            SetRax(task->registers.rax);
            SetRbx(task->registers.rbx);
            SetRcx(task->registers.rcx);
            SetRdx(task->registers.rdx);
            SetRsi(task->registers.rsi);
            SetRdi(task->registers.rdi);
            SetRbp(task->registers.rbp);
            SetRsp(task->registers.rsp);
            SetR8(task->registers.r8);
            SetR9(task->registers.r9);
            SetR10(task->registers.r10);
            SetR11(task->registers.r11);
            SetR12(task->registers.r12);
            SetR13(task->registers.r13);
            SetR14(task->registers.r14);
            SetR15(task->registers.r15);

            //rip = stack;
            rip = task->EntryPoint;
        }else{
            SetRax(0);
            SetRbx(0);
            SetRcx(0);
            SetRdx(0);
            SetRsi(0);
            SetRdi(0);
            SetRbp(0);
            SetRsp(0);
            SetR8(0);
            SetR9(0);
            SetR10(0);
            SetR11(0);
            SetR12(0);
            SetR13(0);
            SetR14(0);
            SetR15(0);
            rip = task->EntryPoint;
            task->IsInit = true;
        }
    }
    return rip;

}

void Schedule::AddTask(void* EntryPoint, size_t Size){ 
    //realloc task table
    size_t SizeNewTaskTable = sizeof(Tasks) + sizeof(Task); //add size of the actual table and add one colone so the size of the colone is the size of Task
    void* NewTaskTableAdress = malloc(SizeNewTaskTable); 
    memcpy(NewTaskTableAdress, (void*)Tasks, SizeNewTaskTable);
    void* OldTask = Tasks;
    Tasks = (Task**)NewTaskTableAdress;
    if(OldTask != 0){
        free(OldTask);
    }
    
    Tasks[PID]->EntryPoint = EntryPoint;
    globalPageTableManager.MapUserspaceMemory(EntryPoint);
    Tasks[PID]->Stack = malloc(Size);

    for(uint64_t i = (uint64_t)Tasks[PID]->Stack; i < (uint64_t)Tasks[PID]->Stack + Size; i+= 0x1000){
        globalPageTableManager.MapUserspaceMemory((void*)i);
    }
    
    Tasks[PID]->PID = PID;
    PID++;
}

void Schedule::EnabledScheduler(){
    IsEnabled = true;
    EnableSystemCall(); 
    JumpIntoUserspace(Tasks[0]->EntryPoint, Tasks[0]->Stack);
}
