#ifndef _IPC_H
#define _IPC_H 1

#include <kot/sys.h>
#include <kot/types.h>

static inline KResult CallIPC(){
    parameters_t Parameters;
    return Sys_CIP(KotSpecificData.IPCHandler, &Parameters);
}


#endif