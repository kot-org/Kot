#include <syscall/syscall.h>


KResult Sys_Kot_CreateMemoryField(struct SyscallStack* Registers, kthread_t* Thread);
KResult Sys_Kot_AcceptMemoryField(struct SyscallStack* Registers, kthread_t* Thread);
KResult Sys_Kot_CloseMemoryField(struct SyscallStack* Registers, kthread_t* Thread);
KResult Sys_Kot_GetInfoMemoryField(struct SyscallStack* Registers, kthread_t* Thread);
KResult Sys_Kot_CreateProc(struct SyscallStack* Registers, kthread_t* Thread);
KResult Sys_Kot_Fork(struct SyscallStack* Registers, kthread_t* Thread);
KResult Sys_Kot_CloseProc(struct SyscallStack* Registers, kthread_t* Thread);
KResult Sys_Kot_Close(struct SyscallStack* Registers, kthread_t* Thread);
KResult Sys_Kot_Exit(struct SyscallStack* Registers, kthread_t* Thread);
KResult Sys_Kot_Pause(struct SyscallStack* Registers, kthread_t* Thread);
KResult Sys_Kot_UnPause(struct SyscallStack* Registers, kthread_t* Thread);
KResult Sys_Kot_Map(struct SyscallStack* Registers, kthread_t* Thread);
KResult Sys_Kot_Unmap(struct SyscallStack* Registers, kthread_t* Thread);
KResult Sys_Kot_GetPhysical(struct SyscallStack* Registers, kthread_t* Thread);
KResult Sys_Kot_Event_Create(struct SyscallStack* Registers, kthread_t* Thread);
KResult Sys_Kot_Event_Bind(struct SyscallStack* Registers, kthread_t* Thread);
KResult Sys_Kot_Event_Unbind(struct SyscallStack* Registers, kthread_t* Thread);
KResult Sys_Kot_Event_Trigger(struct SyscallStack* Registers, kthread_t* Thread);
KResult Sys_Kot_Event_Close(struct SyscallStack* Registers, kthread_t* Thread);
KResult Sys_Kot_CreateThread(struct SyscallStack* Registers, kthread_t* Thread);
KResult Sys_Kot_DuplicateThread(struct SyscallStack* Registers, kthread_t* Thread);
KResult Sys_Kot_ExecThread(struct SyscallStack* Registers, kthread_t* Thread);
KResult Sys_Kot_Keyhole_CloneModify(struct SyscallStack* Registers, kthread_t* Thread);
KResult Sys_Kot_Keyhole_Verify(struct SyscallStack* Registers, kthread_t* Thread);
KResult Sys_Kot_TCB_Set(struct SyscallStack* Registers, kthread_t* Thread);
KResult Sys_Kot_Thread_Info_Get(struct SyscallStack* Registers, kthread_t* Thread);
KResult Sys_Kot_WaitPID(struct SyscallStack* Registers, kthread_t* Thread);
KResult Sys_Kot_Logs(struct SyscallStack* Registers, kthread_t* Thread);