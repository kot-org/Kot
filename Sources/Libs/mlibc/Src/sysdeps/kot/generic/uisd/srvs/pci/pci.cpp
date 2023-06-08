#include <kot/uisd/srvs/pci.h>
#include <stdlib.h>
#include <string.h>

extern "C" {

kot_thread_t kot_srv_pci_callback_thread = NULL;

void kot_Srv_Pci_Initialize(){
    kot_uisd_pci_t* PciData = (kot_uisd_pci_t*)kot_FindControllerUISD(ControllerTypeEnum_PCI);
    kot_process_t proc = kot_Sys_GetProcess();

    kot_thread_t PciThreadKeyCallback = NULL;
    kot_Sys_CreateThread(proc, (void*)&kot_Srv_Pci_Callback, PriviledgeDriver, NULL, &PciThreadKeyCallback);
    kot_srv_pci_callback_thread = kot_MakeShareableThreadToProcess(PciThreadKeyCallback, PciData->ControllerHeader.Process);
}

void kot_Srv_Pci_Callback(KResult Status, struct kot_srv_pci_callback_t* Callback, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3){
    Callback->Status = Callback->Handler(Status, Callback, GP0, GP1, GP2, GP3);

    if(Callback->IsAwait){
        kot_Sys_Unpause(Callback->Self);
    }
        
    kot_Sys_Close(KSUCCESS);
}

/* CountDevices */
KResult Srv_Pci_CountDevices_Callback(KResult Status, struct kot_srv_pci_callback_t* Callback, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3){
    if(Status == KSUCCESS){
        Callback->Data = GP0;
        Callback->Size = sizeof(size64_t);
    }
    return Status;
}

struct kot_srv_pci_callback_t* kot_Srv_Pci_CountDevices(kot_srv_pci_search_parameters_t* SearchParameters, bool IsAwait){
    if(!kot_srv_pci_callback_thread) kot_Srv_Pci_Initialize();
    kot_uisd_pci_t* PciData = (kot_uisd_pci_t*)kot_FindControllerUISD(ControllerTypeEnum_PCI);

    kot_thread_t self = kot_Sys_GetThread();

    struct kot_srv_pci_callback_t* callback = (struct kot_srv_pci_callback_t*)malloc(sizeof(struct kot_srv_pci_callback_t));
    callback->Self = self;
    callback->Data = NULL;
    callback->Size = NULL;
    callback->IsAwait = IsAwait;
    callback->Status = KBUSY;
    callback->Handler = &Srv_Pci_CountDevices_Callback; 

    struct kot_ShareDataWithArguments_t data;
    data.Data = (void*)SearchParameters;
    data.Size = sizeof(kot_srv_pci_search_parameters_t);
    data.ParameterPosition = 0x2;

    struct kot_arguments_t parameters;
    parameters.arg[0] = kot_srv_pci_callback_thread;
    parameters.arg[1] = (uint64_t)callback;

    KResult Status = kot_Sys_ExecThread(PciData->CountDevices, &parameters, ExecutionTypeQueu, &data);
    if(Status == KSUCCESS && IsAwait){
        kot_Sys_Pause(false);
    }
    return callback;
}

/* FindDevice */
KResult Srv_Pci_FindDevice_Callback(KResult Status, struct kot_srv_pci_callback_t* Callback, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3){
    if(Status == KSUCCESS){
        Callback->Data = GP0;
        Callback->Size = sizeof(size64_t);
    }
    return Status;
}

struct kot_srv_pci_callback_t* kot_Srv_Pci_FindDevice(kot_srv_pci_search_parameters_t* SearchParameters, uint64_t Index, bool IsAwait){
    if(!kot_srv_pci_callback_thread) kot_Srv_Pci_Initialize();
    kot_uisd_pci_t* PciData = (kot_uisd_pci_t*)kot_FindControllerUISD(ControllerTypeEnum_PCI);

    kot_thread_t self = kot_Sys_GetThread();

    struct kot_srv_pci_callback_t* callback = (struct kot_srv_pci_callback_t*)malloc(sizeof(struct kot_srv_pci_callback_t));
    callback->Self = self;
    callback->Data = NULL;
    callback->Size = NULL;
    callback->IsAwait = IsAwait;
    callback->Status = KBUSY;
    callback->Handler = &Srv_Pci_FindDevice_Callback; 

    struct kot_ShareDataWithArguments_t data;
    data.Data = (void*)SearchParameters;
    data.Size = sizeof(kot_srv_pci_search_parameters_t);
    data.ParameterPosition = 0x2;

    struct kot_arguments_t parameters;
    parameters.arg[0] = kot_srv_pci_callback_thread;
    parameters.arg[1] = (uint64_t)callback;
    // arg  2 is reserved for kot_srv_pci_search_parameters_t
    parameters.arg[3] = Index;

    KResult Status = kot_Sys_ExecThread(PciData->FindDevice, &parameters, ExecutionTypeQueu, &data);
    if(Status == KSUCCESS && IsAwait){
        kot_Sys_Pause(false);
    }
    return callback;
}

/* GetInfoDevice */
KResult Srv_Pci_GetInfoDevice_Callback(KResult Status, struct kot_srv_pci_callback_t* Callback, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3){
    if(Status == KSUCCESS){
        Callback->Data = (uint64_t)malloc(sizeof(kot_srv_pci_device_info_t));
        memcpy((void*)Callback->Data, (void*)GP0, sizeof(kot_srv_pci_device_info_t));
        Callback->Size = sizeof(kot_srv_pci_device_info_t);
    }
    return Status;
}

struct kot_srv_pci_callback_t* kot_Srv_Pci_GetInfoDevice(kot_PCIDeviceID_t Device, bool IsAwait){
    if(!kot_srv_pci_callback_thread) kot_Srv_Pci_Initialize();
    kot_uisd_pci_t* PciData = (kot_uisd_pci_t*)kot_FindControllerUISD(ControllerTypeEnum_PCI);

    kot_thread_t self = kot_Sys_GetThread();

    struct kot_srv_pci_callback_t* callback = (struct kot_srv_pci_callback_t*)malloc(sizeof(struct kot_srv_pci_callback_t));
    callback->Self = self;
    callback->Data = NULL;
    callback->Size = NULL;
    callback->IsAwait = IsAwait;
    callback->Status = KBUSY;
    callback->Handler = &Srv_Pci_GetInfoDevice_Callback; 

    struct kot_arguments_t parameters;
    parameters.arg[0] = kot_srv_pci_callback_thread;
    parameters.arg[1] = (uint64_t)callback;
    parameters.arg[2] = Device;

    KResult Status = kot_Sys_ExecThread(PciData->GetInfoDevice, &parameters, ExecutionTypeQueu, NULL);
    if(Status == KSUCCESS && IsAwait){
        kot_Sys_Pause(false);
    }
    return callback;
}

/* GetBAR */
KResult Srv_Pci_GetBAR_Callback(KResult Status, struct kot_srv_pci_callback_t* Callback, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3){
    if(Status == KSUCCESS){
        Callback->Data = (uint64_t)malloc(sizeof(kot_srv_pci_bar_info_t));
        memcpy((void*)Callback->Data, (void*)GP0, sizeof(kot_srv_pci_bar_info_t));
        Callback->Size = sizeof(kot_srv_pci_bar_info_t);
    }
    return Status;
}

struct kot_srv_pci_callback_t* kot_Srv_Pci_GetBAR(kot_PCIDeviceID_t Device, uint8_t BarIndex, bool IsAwait){
    if(!kot_srv_pci_callback_thread) kot_Srv_Pci_Initialize();
    kot_uisd_pci_t* PciData = (kot_uisd_pci_t*)kot_FindControllerUISD(ControllerTypeEnum_PCI);

    kot_thread_t self = kot_Sys_GetThread();

    struct kot_srv_pci_callback_t* callback = (struct kot_srv_pci_callback_t*)malloc(sizeof(struct kot_srv_pci_callback_t));
    callback->Self = self;
    callback->Data = NULL;
    callback->Size = NULL;
    callback->IsAwait = IsAwait;
    callback->Status = KBUSY;
    callback->Handler = &Srv_Pci_GetBAR_Callback; 

    struct kot_arguments_t parameters;
    parameters.arg[0] = kot_srv_pci_callback_thread;
    parameters.arg[1] = (uint64_t)callback;
    parameters.arg[2] = Device;
    parameters.arg[3] = BarIndex;

    KResult Status = kot_Sys_ExecThread(PciData->GetBARDevice, &parameters, ExecutionTypeQueu, NULL);

    if(Status == KSUCCESS && IsAwait){
        kot_Sys_Pause(false);
    }
    return callback;
}

/* BindMSI */
KResult Srv_Pci_BindMSI_Callback(KResult Status, struct kot_srv_pci_callback_t* Callback, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3){
    if(Status == KSUCCESS){
        Callback->Data = GP0;
    }
    return Status;
}

struct kot_srv_pci_callback_t* kot_Srv_Pci_BindMSI(kot_PCIDeviceID_t Device, uint8_t IRQVector, uint8_t Processor, uint16_t LocalDeviceVector, bool IsAwait){
    if(!kot_srv_pci_callback_thread) kot_Srv_Pci_Initialize();
    kot_uisd_pci_t* PciData = (kot_uisd_pci_t*)kot_FindControllerUISD(ControllerTypeEnum_PCI);

    kot_thread_t self = kot_Sys_GetThread();

    struct kot_srv_pci_callback_t* callback = (struct kot_srv_pci_callback_t*)malloc(sizeof(struct kot_srv_pci_callback_t));
    callback->Self = self;
    callback->Data = NULL;
    callback->Size = NULL;
    callback->IsAwait = IsAwait;
    callback->Status = KBUSY;
    callback->Handler = &Srv_Pci_BindMSI_Callback; 

    struct kot_arguments_t parameters;
    parameters.arg[0] = kot_srv_pci_callback_thread;
    parameters.arg[1] = (uint64_t)callback;
    parameters.arg[2] = Device;
    parameters.arg[3] = IRQVector;
    parameters.arg[4] = Processor;
    parameters.arg[5] = LocalDeviceVector;

    KResult Status = kot_Sys_ExecThread(PciData->BindMSI, &parameters, ExecutionTypeQueu, NULL);

    if(Status == KSUCCESS && IsAwait){
        kot_Sys_Pause(false);
    }
    return callback;
}

/* UnbindMSI */
KResult Srv_Pci_UnbindMSI_Callback(KResult Status, struct kot_srv_pci_callback_t* Callback, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3){
    return Status;
}

struct kot_srv_pci_callback_t* kot_Srv_Pci_UnbindMSI(kot_PCIDeviceID_t Device, uint16_t LocalDeviceVector, bool IsAwait){
    if(!kot_srv_pci_callback_thread) kot_Srv_Pci_Initialize();
    kot_uisd_pci_t* PciData = (kot_uisd_pci_t*)kot_FindControllerUISD(ControllerTypeEnum_PCI);

    kot_thread_t self = kot_Sys_GetThread();

    struct kot_srv_pci_callback_t* callback = (struct kot_srv_pci_callback_t*)malloc(sizeof(struct kot_srv_pci_callback_t));
    callback->Self = self;
    callback->Data = NULL;
    callback->Size = NULL;
    callback->IsAwait = IsAwait;
    callback->Status = KBUSY;
    callback->Handler = &Srv_Pci_UnbindMSI_Callback; 

    struct kot_arguments_t parameters;
    parameters.arg[0] = kot_srv_pci_callback_thread;
    parameters.arg[1] = (uint64_t)callback;
    parameters.arg[2] = Device;
    parameters.arg[3] = LocalDeviceVector;

    KResult Status = kot_Sys_ExecThread(PciData->UnbindMSI, &parameters, ExecutionTypeQueu, NULL);

    if(Status == KSUCCESS && IsAwait){
        kot_Sys_Pause(false);
    }
    return callback;
}

/* ConfigReadWord */
KResult Srv_Pci_ConfigReadWord_Callback(KResult Status, struct kot_srv_pci_callback_t* Callback, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3){
    if(Status == KSUCCESS){
        Callback->Data = GP0;
        Callback->Size = sizeof(uint16_t);
    }
    return Status;
}

struct kot_srv_pci_callback_t* kot_Srv_Pci_ConfigReadWord(kot_PCIDeviceID_t Device, uint16_t Offset, bool IsAwait){
    if(!kot_srv_pci_callback_thread) kot_Srv_Pci_Initialize();
    kot_uisd_pci_t* PciData = (kot_uisd_pci_t*)kot_FindControllerUISD(ControllerTypeEnum_PCI);

    kot_thread_t self = kot_Sys_GetThread();

    struct kot_srv_pci_callback_t* callback = (struct kot_srv_pci_callback_t*)malloc(sizeof(struct kot_srv_pci_callback_t));
    callback->Self = self;
    callback->Data = NULL;
    callback->Size = NULL;
    callback->IsAwait = IsAwait;
    callback->Status = KBUSY;
    callback->Handler = &Srv_Pci_ConfigReadWord_Callback; 

    struct kot_arguments_t parameters;
    parameters.arg[0] = kot_srv_pci_callback_thread;
    parameters.arg[1] = (uint64_t)callback;
    parameters.arg[2] = Device;
    parameters.arg[3] = Offset;

    KResult Status = kot_Sys_ExecThread(PciData->ConfigReadWord, &parameters, ExecutionTypeQueu, NULL);

    if(Status == KSUCCESS && IsAwait){
        kot_Sys_Pause(false);
    }
    return callback;
}

/* ConfigWriteWord */
KResult Srv_Pci_ConfigWriteWord_Callback(KResult Status, struct kot_srv_pci_callback_t* Callback, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3){
    return Status;
}

struct kot_srv_pci_callback_t* kot_Srv_Pci_ConfigWriteWord(kot_PCIDeviceID_t Device, uint16_t Offset, uint16_t Value, bool IsAwait){
    if(!kot_srv_pci_callback_thread) kot_Srv_Pci_Initialize();
    kot_uisd_pci_t* PciData = (kot_uisd_pci_t*)kot_FindControllerUISD(ControllerTypeEnum_PCI);

    kot_thread_t self = kot_Sys_GetThread();

    struct kot_srv_pci_callback_t* callback = (struct kot_srv_pci_callback_t*)malloc(sizeof(struct kot_srv_pci_callback_t));
    callback->Self = self;
    callback->Data = NULL;
    callback->Size = NULL;
    callback->IsAwait = IsAwait;
    callback->Status = KBUSY;
    callback->Handler = &Srv_Pci_ConfigWriteWord_Callback; 

    struct kot_arguments_t parameters;
    parameters.arg[0] = kot_srv_pci_callback_thread;
    parameters.arg[1] = (uint64_t)callback;
    parameters.arg[2] = Device;
    parameters.arg[3] = Offset;
    parameters.arg[4] = Value;

    KResult Status = kot_Sys_ExecThread(PciData->ConfigWriteWord, &parameters, ExecutionTypeQueu, NULL);

    if(Status == KSUCCESS && IsAwait){
        kot_Sys_Pause(false);
    }
    return callback;
}

}