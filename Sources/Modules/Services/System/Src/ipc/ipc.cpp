#include <ipc/ipc.h>

kthread_t threadIPC;

kthread_t IPCInitialize(){
    kthread_t IPCThreadKey;
    uint64_t IPCKeyFlags = NULL;
    Keyhole_SetFlag(&IPCKeyFlags, KeyholeFlagPresent, true);
    Keyhole_SetFlag(&IPCKeyFlags, KeyholeFlagDataTypeThreadIsExecutableAsCIP, true);

    kprocess_t process;
    SYS_GetProcessKey(&process);

    Sys_CreateThread(process, (uintptr_t)IPCHandler, PriviledgeService, NULL, &threadIPC);
    Sys_Keyhole_CloneModify(threadIPC, &IPCThreadKey, NULL, IPCKeyFlags);

    return IPCThreadKey;
}

KResult IPCHandler(){
    Printlog("IPC");
    for(int i = 0; i < 0x100000; i++);
    SYS_Exit(NULL, KSUCCESS);
}