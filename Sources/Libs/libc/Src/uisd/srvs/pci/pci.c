#include <kot/uisd/srvs/pci.h>

thread_t SrvPciCallbackThread = NULL;
uisd_pci_t* PciData = NULL;

void SrvPciInitialize() {
    PciData = (uisd_pci_t*) FindControllerUISD(ControllerTypeEnum_PCI);
    
    if(PciData != NULL) {
        process_t proc = Sys_GetProcess();

        thread_t PciThreadKeyCallback = NULL;
        Sys_Createthread(proc, &SrvPciCallback, PriviledgeDriver, &PciThreadKeyCallback);
        SrvPciCallbackThread = MakeShareableThreadToProcess(PciThreadKeyCallback, PciData->ControllerHeader.Process);
    } else {
        Sys_Close(KFAIL);
    }
}

void SrvPciCallback(KResult Status, srv_pci_callback_t* Callback, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3) {
    Callback->Status = Callback->Handler(Status, Callback, GP0, GP1, GP2, GP3);

    if(Callback->IsAwait){
        Sys_Unpause(Callback->Self);
    }
        
    Sys_Close(KSUCCESS);
}

/* GetBARNum */
KResult SrvPciGetBARNumCallback(KResult Status, srv_pci_callback_t* Callback, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3) {
    if(Status == KSUCCESS) {
        Callback->Data = GP0;
    }
    return Status;
}

srv_pci_callback_t* SrvPciGetBARNum(srv_pci_bar_t* bar, bool IsAwait) {
    if(!SrvPciCallbackThread) SrvPciInitialize();

    thread_t self = Sys_Getthread();

    srv_pci_callback_t* callback = (srv_pci_callback_t*) malloc(sizeof(srv_pci_callback_t));
    callback->Self = self;
    callback->Data = bar;
    callback->IsAwait = IsAwait;
    callback->Status = KBUSY;
    callback->Handler = &SrvPciGetBARNumCallback; 

    struct arguments_t parameters;
    parameters.arg[0] = SrvPciCallbackThread;
    parameters.arg[1] = callback;

    KResult Status = Sys_Execthread(PciData->GetBARNum, &parameters, ExecutionTypeQueu, NULL);

    if(Status == KSUCCESS && IsAwait)
        Sys_Pause(false);
    return callback;
}