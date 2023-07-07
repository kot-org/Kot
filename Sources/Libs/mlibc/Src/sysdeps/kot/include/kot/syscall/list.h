#if defined(__cplusplus)
extern "C" {
#endif

/* Std */
#define KSys_Std_Start 0x0
#define KSys_Std_TCB_Set KSys_Std_Start + 0x0
#define KSys_Std_Futex_Wait KSys_Std_Start + 0x1
#define KSys_Std_Futex_Wake KSys_Std_Start + 0x2
#define KSys_Std_Vm_Map KSys_Std_Start + 0x3
#define KSys_Std_Vm_Unmap KSys_Std_Start + 0x4
#define KSys_Std_Vm_Protect KSys_Std_Start + 0x5
#define KSys_Std_Exit KSys_Std_Start + 0x6
#define KSys_Std_Thread_Exit KSys_Std_Start + 0x7
#define KSys_Std_Sigprocmask KSys_Std_Start + 0x8
#define KSys_Std_Sigaction KSys_Std_Start + 0x9
#define KSys_Std_Fork KSys_Std_Start + 0xA
#define KSys_Std_Wait_PID KSys_Std_Start + 0xB
#define KSys_Std_Get_PID KSys_Std_Start + 0xC
#define KSys_Std_Kill KSys_Std_Start + 0xD

/* Kot */
#define KSys_Kot_Start KSys_Std_Kill
#define KSys_Kot_CreateMemoryField KSys_Kot_Start + 0x0
#define KSys_Kot_AcceptMemoryField KSys_Kot_Start + 0x1
#define KSys_Kot_CloseMemoryField KSys_Kot_Start + 0x2
#define KSys_Kot_GetInfoMemoryField KSys_Kot_Start + 0x3

#define KSys_Kot_CreateProc KSys_Kot_Start + 0x4
#define KSys_Kot_Fork KSys_Kot_Start + 0x5
#define KSys_Kot_CloseProc KSys_Kot_Start + 0x6

#define KSys_Kot_Close KSys_Kot_Start + 0x7
#define KSys_Kot_Exit KSys_Kot_Start + 0x8
#define KSys_Kot_Pause KSys_Kot_Start + 0x9
#define KSys_Kot_UnPause KSys_Kot_Start + 0xA

#define KSys_Kot_Map KSys_Kot_Start + 0xB
#define KSys_Kot_Unmap KSys_Kot_Start + 0xC
#define KSys_Kot_GetPhysical KSys_Kot_Start + 0xD

#define KSys_Kot_Event_Create KSys_Kot_Start + 0xE
#define KSys_Kot_Event_Bind KSys_Kot_Start + 0xF
#define KSys_Kot_Event_Unbind KSys_Kot_Start + 0x10
#define KSys_Kot_Event_Trigger KSys_Kot_Start + 0x11
#define KSys_Kot_Event_Close KSys_Kot_Start + 0x12

#define KSys_Kot_CreateThread KSys_Kot_Start + 0x13
#define KSys_Kot_DuplicateThread KSys_Kot_Start + 0x14
#define KSys_Kot_ExecThread KSys_Kot_Start + 0x15

#define KSys_Kot_Keyhole_CloneModify KSys_Kot_Start + 0x16
#define KSys_Kot_Keyhole_Verify KSys_Kot_Start + 0x17

#define KSys_Kot_TCB_Set KSys_Kot_Start + 0x18

#define KSys_Kot_Thread_Info_Get KSys_Kot_Start + 0x19

#define KSys_Kot_WaitPID KSys_Kot_Start + 0x20

#define KSys_Kot_Logs KSys_Kot_Start + 0x21

#define Syscall_Count KSys_Kot_Logs + 1

#if defined(__cplusplus)
} 
#endif

