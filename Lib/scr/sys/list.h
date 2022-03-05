#pragma once

#define Sys_CreatShareMemory 0x0
#define Sys_GetShareMemory 0x1
#define Sys_FreeShareMemory 0x2
#define Sys_StackShareMemory 0x3

#define Sys_Fork 0x4
#define Sys_CreatProc 0x5
#define Sys_CloseProc 0x6

#define Sys_Exit 0x7
#define Sys_Pause 0x8
#define Sys_UnPause 0x9

#define Sys_Map 0xA
#define Sys_Unmap 0xB

#define Sys_Event_Creat 0xC
#define Sys_Event_Bind 0xD
#define Sys_Event_Unbind 0xE
#define Sys_Event_Trigger 0xF

#define Sys_CreatThread 0x10
#define Sys_DuplicateThread 0x11
#define Sys_ExecThread 0x12

#define Sys_Get_IOPL 0x13