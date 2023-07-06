#include <syscall/syscall.h>

int Sys_Std_TCB_Set(struct SyscallStack* Registers, kthread_t* Thread);

int Sys_Std_Futex_TID(struct SyscallStack* Registers, kthread_t* Thread);

int Sys_Std_Futex_Wait(struct SyscallStack* Registers, kthread_t* Thread);

int Sys_Std_Futex_Wake(struct SyscallStack* Registers, kthread_t* Thread);

int Sys_Std_Vm_Map(struct SyscallStack* Registers, kthread_t* Thread);

int Sys_Std_Vm_Unmap(struct SyscallStack* Registers, kthread_t* Thread);

int Sys_Std_Vm_Protect(struct SyscallStack* Registers, kthread_t* Thread);

int Sys_Std_Exit(struct SyscallStack* Registers, kthread_t* Thread);

int Sys_Std_Thread_Exit(struct SyscallStack* Registers, kthread_t* Thread);

int Sys_Std_Sigprocmask(struct SyscallStack* Registers, kthread_t* Thread);

int Sys_Std_Sigaction(struct SyscallStack* Registers, kthread_t* Thread);

int Sys_Std_Fork(struct SyscallStack* Registers, kthread_t* Thread);

int Sys_Std_Wait_PID(struct SyscallStack* Registers, kthread_t* Thread);

int Sys_Std_Get_PID(struct SyscallStack* Registers, kthread_t* Thread);

int Sys_Std_Kill(struct SyscallStack* Registers, kthread_t* Thread);