#include <kot/uisd/srvs/pci.h>

thread_t SrvPciCallbackThread = NULL;
uisd_pci_t* PciData = NULL;

void Srv_Pci_Initialize(){
    PciData = (uisd_pci_t*)FindControllerUISD(ControllerTypeEnum_PCI);
    
    if(PciData != NULL){
        process_t proc = Sys_GetProcess();

        thread_t PciThreadKeyCallback = NULL;
        Sys_Createthread(proc, &Srv_Pci_Callback, PriviledgeDriver, &PciThreadKeyCallback);
        SrvPciCallbackThread = MakeShareableThreadToProcess(PciThreadKeyCallback, PciData->ControllerHeader.Process);
    } else{
        Sys_Close(KFAIL);
    }
}

void Srv_Pci_Callback(KResult Status, struct srv_pci_callback_t* Callback, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3){
    Callback->Status = Callback->Handler(Status, Callback, GP0, GP1, GP2, GP3);

    if(Callback->IsAwait){
        Sys_Unpause(Callback->Self);
    }
        
    Sys_Close(KSUCCESS);
}

/* CountDevices */
KResult Srv_Pci_CountDevices_Callback(KResult Status, struct srv_pci_callback_t* Callback, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3){
    if(Status == KSUCCESS){
        Callback->Data = GP0;
        Callback->Size = sizeof(size64_t);
    }
    return Status;
}

struct srv_pci_callback_t* Srv_Pci_CountDevices(srv_pci_search_parameters_t* SearchParameters, bool IsAwait){
    if(!SrvPciCallbackThread) Srv_Pci_Initialize();

    thread_t self = Sys_Getthread();

    struct srv_pci_callback_t* callback = (struct srv_pci_callback_t*)malloc(sizeof(struct srv_pci_callback_t));
    callback->Self = self;
    callback->Data = NULL;
    callback->Size = NULL;
    callback->IsAwait = IsAwait;
    callback->Status = KBUSY;
    callback->Handler = &Srv_Pci_CountDevices_Callback; 

    struct ShareDataWithArguments_t data;
    data.Data = SearchParameters;
    data.Size = sizeof(srv_pci_search_parameters_t);
    data.ParameterPosition = 0x2;

    struct arguments_t parameters;
    parameters.arg[0] = SrvPciCallbackThread;
    parameters.arg[1] = callback;

    KResult Status = Sys_Execthread(PciData->CountDevices, &parameters, ExecutionTypeQueu, &data);
    if(Status == KSUCCESS && IsAwait){
        Sys_Pause(false);
    }
    return callback;
}

/* FindDevice */
KResult Srv_Pci_FindDevice_Callback(KResult Status, struct srv_pci_callback_t* Callback, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3){
    if(Status == KSUCCESS){
        Callback->Data = GP0;
        Callback->Size = sizeof(size64_t);
    }
    return Status;
}

struct srv_pci_callback_t* Srv_Pci_FindDevice(srv_pci_search_parameters_t* SearchParameters, uint64_t Index, bool IsAwait){
    if(!SrvPciCallbackThread) Srv_Pci_Initialize();

    thread_t self = Sys_Getthread();

    struct srv_pci_callback_t* callback = (struct srv_pci_callback_t*)malloc(sizeof(struct srv_pci_callback_t));
    callback->Self = self;
    callback->Data = NULL;
    callback->Size = NULL;
    callback->IsAwait = IsAwait;
    callback->Status = KBUSY;
    callback->Handler = &Srv_Pci_FindDevice_Callback; 

    struct ShareDataWithArguments_t data;
    data.Data = SearchParameters;
    data.Size = sizeof(srv_pci_search_parameters_t);
    data.ParameterPosition = 0x2;

    struct arguments_t parameters;
    parameters.arg[0] = SrvPciCallbackThread;
    parameters.arg[1] = callback;
    // arg  2 is reserved for srv_pci_search_parameters_t
    parameters.arg[3] = Index;

    KResult Status = Sys_Execthread(PciData->FindDevice, &parameters, ExecutionTypeQueu, &data);
    if(Status == KSUCCESS && IsAwait){
        Sys_Pause(false);
    }
    return callback;
}

/* GetInfoDevice */
KResult Srv_Pci_GetInfoDevice_Callback(KResult Status, struct srv_pci_callback_t* Callback, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3){
    if(Status == KSUCCESS){
        Callback->Data = malloc(sizeof(srv_pci_device_info_t));
        memcpy(Callback->Data, (uintptr_t)GP0, sizeof(srv_pci_device_info_t));
        Callback->Size = sizeof(srv_pci_device_info_t);
    }
    return Status;
}

struct srv_pci_callback_t* Srv_Pci_GetInfoDevice(PCIDeviceID_t Device, bool IsAwait){
    if(!SrvPciCallbackThread) Srv_Pci_Initialize();

    thread_t self = Sys_Getthread();

    struct srv_pci_callback_t* callback = (struct srv_pci_callback_t*)malloc(sizeof(struct srv_pci_callback_t));
    callback->Self = self;
    callback->Data = NULL;
    callback->Size = NULL;
    callback->IsAwait = IsAwait;
    callback->Status = KBUSY;
    callback->Handler = &Srv_Pci_GetInfoDevice_Callback; 

    struct arguments_t parameters;
    parameters.arg[0] = SrvPciCallbackThread;
    parameters.arg[1] = callback;
    parameters.arg[2] = Device;

    KResult Status = Sys_Execthread(PciData->GetInfoDevice, &parameters, ExecutionTypeQueu, NULL);
    if(Status == KSUCCESS && IsAwait){
        Sys_Pause(false);
    }
    return callback;
}

/* GetBAR */
KResult Srv_Pci_GetBAR_Callback(KResult Status, struct srv_pci_callback_t* Callback, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3){
    if(Status == KSUCCESS){
        Callback->Data = malloc(sizeof(srv_pci_bar_info_t));
        memcpy(Callback->Data, (uintptr_t)GP0, sizeof(srv_pci_bar_info_t));
        Callback->Size = sizeof(srv_pci_bar_info_t);
    }
    return Status;
}

struct srv_pci_callback_t* Srv_Pci_GetBAR(PCIDeviceID_t Device, uint8_t BarIndex, bool IsAwait){
    if(!SrvPciCallbackThread) Srv_Pci_Initialize();

    thread_t self = Sys_Getthread();

    struct srv_pci_callback_t* callback = (struct srv_pci_callback_t*)malloc(sizeof(struct srv_pci_callback_t));
    callback->Self = self;
    callback->Data = NULL;
    callback->Size = NULL;
    callback->IsAwait = IsAwait;
    callback->Status = KBUSY;
    callback->Handler = &Srv_Pci_GetBAR_Callback; 

    struct arguments_t parameters;
    parameters.arg[0] = SrvPciCallbackThread;
    parameters.arg[1] = callback;
    parameters.arg[2] = Device;
    parameters.arg[3] = BarIndex;

    KResult Status = Sys_Execthread(PciData->GetBARDevice, &parameters, ExecutionTypeQueu, NULL);

    if(Status == KSUCCESS && IsAwait){
        Sys_Pause(false);
    }
    return callback;
}

/* SetupMSI */
KResult Srv_Pci_GetBAR_SetupMSI(KResult Status, struct srv_pci_callback_t* Callback, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3){
    return Status;
}

struct srv_pci_callback_t* Srv_Pci_SetupMSI(PCIDeviceID_t Device, uint8_t IRQVector, uint16_t LocalDeviceVector, bool IsAwait){
    if(!SrvPciCallbackThread) Srv_Pci_Initialize();

    thread_t self = Sys_Getthread();

    struct srv_pci_callback_t* callback = (struct srv_pci_callback_t*)malloc(sizeof(struct srv_pci_callback_t));
    callback->Self = self;
    callback->Data = NULL;
    callback->Size = NULL;
    callback->IsAwait = IsAwait;
    callback->Status = KBUSY;
    callback->Handler = &Srv_Pci_GetBAR_SetupMSI; 

    struct arguments_t parameters;
    parameters.arg[0] = SrvPciCallbackThread;
    parameters.arg[1] = callback;
    parameters.arg[2] = Device;
    parameters.arg[3] = IRQVector;
    parameters.arg[4] = LocalDeviceVector;

    KResult Status = Sys_Execthread(PciData->SetupMSI, &parameters, ExecutionTypeQueu, NULL);

    if(Status == KSUCCESS && IsAwait){
        Sys_Pause(false);
    }
    return callback;
}