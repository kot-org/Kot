#pragma once

#define Sys_CreatShareMemory 0x0
#define Sys_GetShareMemory 0x1
#define Sys_FreeShareMemory 0x2

#define Sys_CreatSubtask 0x3
#define Sys_ExecuteSubtask 0x4

#define Sys_Exit 0x5
#define Sys_Pause 0x6

#define Sys_Map 0x7
#define Sys_Unmap 0x8

#define Sys_IRQRedirect 0x9
#define Sys_IRQExit 0xA
#define Sys_IRQDefault 0xB

#define Sys_GetTaskInfo 0xC
#define Sys_SetTaskInfo 0xD

#define Sys_CreatThread 0xE
#define Sys_LaunchThread 0xF
#define Sys_StopThread 0x10

#define Sys_In 0x11
#define Sys_Out 0x12