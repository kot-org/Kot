#ifndef _IPC_H
#define _IPC_H 1

#include <kot/sys.h>
#include <kot/types.h>

static inline KResult CallIPC(char* Name){
    parameters_t Parameters;
    Parameters.Parameter0 = 0;
    return Sys_CIP(KotSpecificData.IPCHandler, &Parameters);
}

static inline KResult CreateIPC(char* Name, kthread_t* Thread){
    parameters_t Parameters;
    Parameters.Parameter0 = 1;
    return Sys_CIP(KotSpecificData.IPCHandler, &Parameters);
}


#endif