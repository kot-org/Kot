#define Syscall_Count 0x14

#define KSys_CreatShareMemory 0x0
#define KSys_GetShareMemory 0x1
#define KSys_FreeShareMemory 0x2
#define KSys_ShareDataUsingStackSpace 0x3

#define KSys_CIP 0x4
#define KSys_CreatProc 0x5
#define KSys_CloseProc 0x6

#define KSys_Rserved 0x7

#define KSys_Exit 0x8
#define KSys_Pause 0x9
#define KSys_UnPause 0xA

#define KSys_Map 0xB
#define KSys_Unmap 0xC

#define KSys_Event_Creat 0xD
#define KSys_Event_Bind 0xE
#define KSys_Event_Unbind 0xF
#define KSys_Event_Trigger 0x10

#define KSys_CreatThread 0x11
#define KSys_DuplicateThread 0x12
#define KSys_ExecThread 0x13

#define KSys_Logs 0x14
