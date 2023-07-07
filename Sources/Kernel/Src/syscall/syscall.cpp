#include <syscall/syscall.h>

static SyscallHandler SyscallHandlers[Syscall_Count] = { 
    /* Std*/
    [KSys_Std_TCB_Set] = reinterpret_cast<SyscallHandler>(Sys_Std_TCB_Set),
    [KSys_Std_Futex_Wait] = reinterpret_cast<SyscallHandler>(Sys_Std_Futex_Wait),
    [KSys_Std_Futex_Wake] = reinterpret_cast<SyscallHandler>(Sys_Std_Futex_Wake),
    [KSys_Std_Vm_Map] = reinterpret_cast<SyscallHandler>(Sys_Std_Vm_Map),
    [KSys_Std_Vm_Unmap] = reinterpret_cast<SyscallHandler>(Sys_Std_Vm_Unmap),
    [KSys_Std_Vm_Protect] = reinterpret_cast<SyscallHandler>(Sys_Std_Vm_Protect),
    [KSys_Std_Exit] = reinterpret_cast<SyscallHandler>(Sys_Std_Exit),
    [KSys_Std_Thread_Exit] = reinterpret_cast<SyscallHandler>(Sys_Std_Thread_Exit),
    [KSys_Std_Sigprocmask] = reinterpret_cast<SyscallHandler>(Sys_Std_Sigprocmask),
    [KSys_Std_Sigaction] = reinterpret_cast<SyscallHandler>(Sys_Std_Sigaction),
    [KSys_Std_Fork] = reinterpret_cast<SyscallHandler>(Sys_Std_Fork),
    [KSys_Std_Wait_PID] = reinterpret_cast<SyscallHandler>(Sys_Std_Wait_PID),
    [KSys_Std_Get_PID] = reinterpret_cast<SyscallHandler>(Sys_Std_Get_PID),
    [KSys_Std_Kill] = reinterpret_cast<SyscallHandler>(Sys_Std_Kill),

    /* Kot specific */
    [KSys_Kot_CreateMemoryField] = Sys_Kot_CreateMemoryField,
    [KSys_Kot_AcceptMemoryField] = Sys_Kot_AcceptMemoryField,
    [KSys_Kot_CloseMemoryField] = Sys_Kot_CloseMemoryField,
    [KSys_Kot_GetInfoMemoryField] = Sys_Kot_GetInfoMemoryField,
    [KSys_Kot_CreateProc] = Sys_Kot_CreateProc,
    [KSys_Kot_Fork] = Sys_Kot_Fork,
    [KSys_Kot_CloseProc] = Sys_Kot_CloseProc,
    [KSys_Kot_Close] = Sys_Kot_Close,
    [KSys_Kot_Exit] = Sys_Kot_Exit,
    [KSys_Kot_Pause] = Sys_Kot_Pause,
    [KSys_Kot_UnPause] = Sys_Kot_UnPause,
    [KSys_Kot_Map] = Sys_Kot_Map,
    [KSys_Kot_Unmap] = Sys_Kot_Unmap,
    [KSys_Kot_GetPhysical] = Sys_Kot_GetPhysical,
    [KSys_Kot_Event_Create] = Sys_Kot_Event_Create,
    [KSys_Kot_Event_Bind] = Sys_Kot_Event_Bind,
    [KSys_Kot_Event_Unbind] = Sys_Kot_Event_Unbind,
    [KSys_Kot_Event_Trigger] = Sys_Kot_Event_Trigger,
    [KSys_Kot_Event_Close] = Sys_Kot_Event_Close,
    [KSys_Kot_CreateThread] = Sys_Kot_CreateThread,
    [KSys_Kot_DuplicateThread] = Sys_Kot_DuplicateThread,
    [KSys_Kot_ExecThread] = Sys_Kot_ExecThread,
    [KSys_Kot_Keyhole_CloneModify] = Sys_Kot_Keyhole_CloneModify,
    [KSys_Kot_Keyhole_Verify] = Sys_Kot_Keyhole_Verify,
    [KSys_Kot_TCB_Set] = Sys_Kot_TCB_Set,
    [KSys_Kot_Thread_Info_Get] = Sys_Kot_Thread_Info_Get,
    [KSys_Kot_WaitPID] = Sys_Kot_WaitPID,
    [KSys_Kot_Logs] = Sys_Kot_Logs,
};

extern "C" void SyscallDispatch(SyscallStack* Registers, kthread_t* Self){
    if(Registers->GlobalPurpose >= Syscall_Count){
        Registers->arg0 = KFAIL;
        return;
    }

    Registers->GlobalPurpose = SyscallHandlers[Registers->GlobalPurpose](Registers, Self);

    return;
}