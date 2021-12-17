#pragma once

#define Sys_CreatShareMemory 0x0
#define Sys_GetShareMemory 0x1

#define Sys_CreatSubtask 0x2
#define Sys_ExecuteSubtask 0x3

#define Sys_Exit 0x4

#define Sys_Map 0x5
#define Sys_Unmap 0x6

#define Sys_IRQRedirect 0x7
#define Sys_IRQDefault 0x8

#define Sys_GetTaskInfo 0x9
#define Sys_SetTaskInfo 0xA

#define Sys_CreatThread 0xB
#define Sys_LaunchThread 0xC