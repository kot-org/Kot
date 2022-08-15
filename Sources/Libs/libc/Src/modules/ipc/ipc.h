#ifndef _IPC_H
#define _IPC_H 1

#include <kot/sys.h>
#include <kot/types.h>
#include <kot/memory.h>
#include <kot/cstring.h>

#define IPCTaskGet      0x0
#define IPCTaskCreate   0x1
#define IPCTaskDelete   0x2

static inline thread CallIPC(char* Name){
    parameters_t Parameters;
    Parameters.Arg0 = IPCTaskGet;
    size_t lenght = strlen(Name);
    if(lenght > 8) lenght = 8;
    Parameters.Arg1 = *(uint64_t*)Name;
    return Sys_IPC(KotSpecificData.IPCHandler, (parameters_t*)&Parameters, false);
}

static inline KResult CreateIPC(char* Name, thread thread){
    parameters_t Parameters;
    Parameters.Arg0 = IPCTaskCreate;
    size_t lenght = strlen(Name);
    if(lenght > 8) lenght = 8;
    Parameters.Arg1 = *(uint64_t*)Name;
    Parameters.Arg2 = (uint64_t)thread;

    return Sys_IPC(KotSpecificData.IPCHandler, (parameters_t*)&Parameters, false);
}


#endif