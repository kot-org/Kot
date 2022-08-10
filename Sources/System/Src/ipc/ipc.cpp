#include "ipc.h"

kthread_t threadIPC;
IPCSaver_t* IPCSaver;

uint64_t IPCMutex;

kthread_t IPCInitialize() {

    kthread_t IPCThreadKey;
    uint64_t IPCKeyFlags = NULL;
    Keyhole_SetFlag(&IPCKeyFlags, KeyholeFlagPresent, true);
    Keyhole_SetFlag(&IPCKeyFlags, KeyholeFlagDataTypeThreadIsExecutableAsIPC, true);

    kprocess_t process;
    Sys_GetProcessKey(&process);

    Sys_CreateThread(process, (uintptr_t)IPCHandler, PriviledgeService, NULL, &threadIPC);
    Sys_Keyhole_CloneModify(threadIPC, &IPCThreadKey, NULL, IPCKeyFlags);

    IPCSaver = (IPCSaver_t*)malloc(sizeof(IPCSaver_t));

    return IPCThreadKey;

}

KResult IPCCreate(uint64_t EncodedName, kthread_t Thread) {
    if (IPCSaver->Main == NULL) {
        IPCSaver->Main = (IPCBlock_t*)malloc(sizeof(IPCBlock_t));
        IPCSaver->Main->Thread = Thread;
        IPCSaver->Main->EncodedName = EncodedName;
        IPCSaver->Last = IPCSaver->Main;
    } else { 
        IPCBlock_t* Block = (IPCBlock_t*)malloc(sizeof(IPCBlock_t));
        Block->Thread = Thread;
        Block->EncodedName = EncodedName;
        IPCSaver->Last->Next = Block;
        IPCSaver->Last = Block;
    }
    IPCSaver->NumberOfTask++;
    return KSUCCESS;
}

kthread_t IPCGet(uint64_t EncodedName) {
    IPCBlock_t* Block = IPCSaver->Main;
    for (uint64_t i = 0; i < IPCSaver->NumberOfTask; i++) {
        if (Block->EncodedName == EncodedName) {
            return Block->Thread;
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
        ReturnValue = (uint64_t)IPCCreate(EncodedName, (kthread_t)GlobalPurpose);
        break;
    case IPCTaskDelete:
        break;
    }
    atomicUnlock(&IPCMutex, 0);
    SYS_Exit(NULL, ReturnValue);
}