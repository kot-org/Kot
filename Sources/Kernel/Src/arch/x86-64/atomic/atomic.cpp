#include <arch/x86-64/atomic/atomic.h>
#include <scheduler/scheduler.h>
#include <arch/x86-64/cpu/cpu.h>

bool Lock(locker_t* locker){
    CPU::DisableInterrupts();
    if(atomicLockAsm(&locker->Mutex, 0)){
        // if(globalTaskManager){
        //     locker->Current = globalTaskManager->ThreadExecutePerCore[CPU::GetAPICID()];
        // }
        CPU::EnableInterrupts();
        return true;
    }
    CPU::EnableInterrupts();
    return false;
}

bool Release(locker_t* locker){
    CPU::DisableInterrupts();
    locker->Current = NULL;
    atomicUnlockAsm(&locker->Mutex, 0);
    CPU::EnableInterrupts();
    return true;
}

void Aquire(locker_t* locker){
    while(!Lock(locker)){
        if(locker->Current){
            SaveTaskAsm();
            JumpToKernelTaskAsm(locker->Current->Regs);
        }
    }
}