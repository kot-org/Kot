#include <kot/uisd/srvs/pci.h>

thread_t SrvPciCallbackThread = NULL;
uisd_pci_t* PciData = NULL;

void SrvPciInitialize() {
    PciData = (uisd_pci_t*)FindControllerUISD(ControllerTypeEnum_PCI);
    
    if(PciData != NULL) {
        process_t proc = Sys_GetProcess();

        thread_t PciThreadKeyCallback = NULL;
        Sys_Createthread(proc, &Srv_Pci_Callback, PriviledgeDriver, &PciThreadKeyCallback);
        SrvPciCallbackThread = MakeShareableThreadToProcess(PciThreadKeyCallback, PciData->ControllerHeader.Process);
    } else {
        Sys_Close(KFAIL);
    }
}

void Srv_Pci_Callback(KResult Status, srv_pci_callback_t* Callback, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3) {
    Callback->Status = Callback->Handler(Status, Callback, GP0, GP1, GP2, GP3);

    if(Callback->IsAwait){
        Sys_Unpause(Callback->Self);
    }
        
    Sys_Close(KSUCCESS);
}

/* GetBARNum */
KResult Srv_Pci_GetBARNum_Callback(KResult Status, srv_pci_callback_t* Callback, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3) {
    if(Status == KSUCCESS) {
        Callback->Data = GP0;
        Callback->Size = sizeof(uint64_t);
    }
    return Status;
}

srv_pci_callback_t* Srv_Pci_GetBARNum(PCIDeviceID_t Device, bool IsAwait) {
    if(!SrvPciCallbackThread) SrvPciInitialize();

    thread_t self = Sys_Getthread();

    srv_pci_callback_t* callback = (srv_pci_callback_t*)malloc(sizeof(srv_pci_callback_t));
    callback->Self = self;
    callback->Data = NULL;
    callback->Size = NULL;
    callback->IsAwait = IsAwait;
    callback->Status = KBUSY;
    callback->Handler = &Srv_Pci_GetBARNum_Callback; 

    struct arguments_t parameters;
    parameters.arg[0] = SrvPciCallbackThread;
    parameters.arg[1] = callback;
    parameters.arg[2] = Device;

    KResult Status = Sys_Execthread(PciData->GetBARNum, &parameters, ExecutionTypeQueu, NULL);

    if(Status == KSUCCESS && IsAwait){
        Sys_Pause(false);
    }
    return callback;
}

/* GetBARType */
KResult Srv_Pci_GetBARType_Callback(KResult Status, srv_pci_callback_t* Callback, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3) {
    if(Status == KSUCCESS) {
        Callback->Data = GP0;
        Callback->Size = sizeof(uint64_t);
    }
    return Status;
}

srv_pci_callback_t* Srv_Pci_GetBARType(PCIDeviceID_t Device, uint8_t BarIndex, bool IsAwait) {
    if(!SrvPciCallbackThread) SrvPciInitialize();

    thread_t self = Sys_Getthread();

    srv_pci_callback_t* callback = (srv_pci_callback_t*)malloc(sizeof(srv_pci_callback_t));
    callback->Self = self;
    callback->Data = NULL;
    callback->Size = NULL;
    callback->IsAwait = IsAwait;
    callback->Status = KBUSY;
    callback->Handler = &Srv_Pci_GetBARType_Callback; 

    struct arguments_t parameters;
    parameters.arg[0] = SrvPciCallbackThread;
    parameters.arg[1] = callback;
    parameters.arg[2] = Device;
    parameters.arg[2] = BarIndex;

    KResult Status = Sys_Execthread(PciData->GetBARType, &parameters, ExecutionTypeQueu, NULL);

    if(Status == KSUCCESS && IsAwait){
        Sys_Pause(false);
    }
    return callback;
}

/* GetBARSize */
KResult Srv_Pci_GetBARSize_Callback(KResult Status, srv_pci_callback_t* Callback, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3) {
    if(Status == KSUCCESS) {
        Callback->Data = GP0;
        Callback->Size = sizeof(uint64_t);
    }
    return Status;
}

srv_pci_callback_t* Srv_Pci_GetBARSize(PCIDeviceID_t Device, uint8_t BarIndex, bool IsAwait) {
    if(!SrvPciCallbackThread) SrvPciInitialize();

    thread_t self = Sys_Getthread();

    srv_pci_callback_t* callback = (srv_pci_callback_t*)malloc(sizeof(srv_pci_callback_t));
    callback->Self = self;
    callback->Data = NULL;
    callback->Size = NULL;
    callback->IsAwait = IsAwait;
    callback->Status = KBUSY;
    callback->Handler = &Srv_Pci_GetBARSize_Callback; 

    struct arguments_t parameters;
    parameters.arg[0] = SrvPciCallbackThread;
    parameters.arg[1] = callback;
    parameters.arg[2] = Device;
    parameters.arg[2] = BarIndex;

    KResult Status = Sys_Execthread(PciData->GetBARSize, &parameters, ExecutionTypeQueu, NULL);

    if(Status == KSUCCESS && IsAwait){
        Sys_Pause(false);
    }
    return callback;
}