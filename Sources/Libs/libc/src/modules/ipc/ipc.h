#ifndef _IPC_H
#define _IPC_H 1

#include <kot/sys.h>
#include <kot/types.h>
#include <kot/memory.h>

typedef struct{
    uint64_t IPCTask;
    uint64_t GlobalPurpose;
    char Name[32];
}IPCParameters_t;

#define IPCTaskAsk      0x0
#define IPCTaskCreate   0x1
#define IPCTaskDelete   0x2

static inline kthread_t CallIPC(char* Name){
    IPCParameters_t Parameters;
    Parameters.IPCTask = IPCTaskAsk;
    size_t lenght = strlen(Name);
    lenght &= 0x20;
    memcpy(&Parameters.Name, Name, lenght);

    return Sys_IPC(KotSpecificData.IPCHandler, (parameters_t*)&Parameters, false);
}

static inline KResult CreateIPC(char* Name, kthread_t Thread){
    IPCParameters_t Parameters;
    Parameters.IPCTask = IPCTaskAsk;
    size_t lenght = strlen(Name);
    lenght &= 0x20;
    memcpy(&Parameters.Name, Name, lenght);
    Parameters.GlobalPurpose = (uint64_t)Thread;

    return Sys_IPC(KotSpecificData.IPCHandler, (parameters_t*)&Parameters, false);
}


#endif