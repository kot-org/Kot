#include "ipc.h"

thread threadIPC;
IPCSaver_t* IPCSaver;

uint64_t IPCMutex;

thread IPCInitialize() {

    thread IPCthreadKey;
    uint64_t IPCKeyFlags = NULL;
    Keyhole_SetFlag(&IPCKeyFlags, KeyholeFlagPresent, true);
    Keyhole_SetFlag(&IPCKeyFlags, KeyholeFlagDataTypethreadIsExecutableAsIPC, true);

    process_t process;
    Sys_GetProcessKey(&process);

    Sys_Createthread(process, (uintptr_t)IPCHandler, PriviledgeService, NULL, &threadIPC);
    Sys_Keyhole_CloneModify(threadIPC, &IPCthreadKey, NULL, IPCKeyFlags);

    IPCSaver = (IPCSaver_t*)malloc(sizeof(IPCSaver_t));

    return IPCthreadKey;

}

KResult IPCCreate(uint64_t EncodedName, thread thread) {
    if (IPCSaver->Main == NULL) {
        IPCSaver->Main = (IPCBlock_t*)malloc(sizeof(IPCBlock_t));
        IPCSaver->Main->thread = thread;
        IPCSaver->Main->EncodedName = EncodedName;
        IPCSaver->Last = IPCSaver->Main;
    } else { 
        IPCBlock_t* Block = (IPCBlock_t*)malloc(sizeof(IPCBlock_t));
        Block->thread = thread;
        Block->EncodedName = EncodedName;
        IPCSaver->Last->Next = Block;
        IPCSaver->Last = Block;
    }
    IPCSaver->NumberOfTask++;
    return KSUCCESS;
}

thread IPCGet(uint64_t EncodedName) {
    IPCBlock_t* Block = IPCSaver->Main;
    for (uint64_t i = 0; i < IPCSaver->NumberOfTask; i++) {
        if (Block->EncodedName == EncodedName) {
            return Block->thread;
        }
        Block = Block->Next;
    }
    return NULL;
}

void IPCHandler(uint64_t IPCTask, uint64_t EncodedName, uint64_t GlobalPurpose) {
    uint64_t ReturnValue = NULL;
    atomicAcquire(&IPCMutex, 0);
    switch (IPCTask) {
    case IPCTaskGet:
        ReturnValue = (uint64_t)IPCGet(EncodedName);
        break;
    case IPCTaskCreate:
        ReturnValue = (uint64_t)IPCCreate(EncodedName, (thread)GlobalPurpose);
        break;
    case IPCTaskDelete:
        break;
    }
    atomicUnlock(&IPCMutex, 0);
    SYS_Exit(NULL, ReturnValue);
}