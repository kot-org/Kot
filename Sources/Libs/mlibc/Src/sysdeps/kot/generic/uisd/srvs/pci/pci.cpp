#include <kot/uisd/srvs/pci.h>
#include <stdlib.h>
#include <string.h>

namespace Kot{
    kot_thread_t SrvPciCallbackThread = NULL;

    void Srv_Pci_Initialize(){
        uisd_pci_t* PciData = (uisd_pci_t*)FindControllerUISD(ControllerTypeEnum_PCI);
        kot_process_t proc = Sys_GetProcess();

        kot_thread_t PciThreadKeyCallback = NULL;
        Sys_CreateThread(proc, (uintptr_t)&Srv_Pci_Callback, PriviledgeDriver, NULL, &PciThreadKeyCallback);
        InitializeThread(PciThreadKeyCallback);
        SrvPciCallbackThread = MakeShareableThreadToProcess(PciThreadKeyCallback, PciData->ControllerHeader.Process);
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
        uisd_pci_t* PciData = (uisd_pci_t*)FindControllerUISD(ControllerTypeEnum_PCI);

        kot_thread_t self = Sys_GetThread();

        struct srv_pci_callback_t* callback = (struct srv_pci_callback_t*)malloc(sizeof(struct srv_pci_callback_t));
        callback->Self = self;
        callback->Data = NULL;
        callback->Size = NULL;
        callback->IsAwait = IsAwait;
        callback->Status = KBUSY;
        callback->Handler = &Srv_Pci_CountDevices_Callback; 

        struct ShareDataWithArguments_t data;
        data.Data = (uintptr_t)SearchParameters;
        data.Size = sizeof(srv_pci_search_parameters_t);
        data.ParameterPosition = 0x2;

        struct kot_arguments_t parameters;
        parameters.arg[0] = SrvPciCallbackThread;
        parameters.arg[1] = (uint64_t)callback;

        KResult Status = Sys_ExecThread(PciData->CountDevices, &parameters, ExecutionTypeQueu, &data);
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
        uisd_pci_t* PciData = (uisd_pci_t*)FindControllerUISD(ControllerTypeEnum_PCI);

        kot_thread_t self = Sys_GetThread();

        struct srv_pci_callback_t* callback = (struct srv_pci_callback_t*)malloc(sizeof(struct srv_pci_callback_t));
        callback->Self = self;
        callback->Data = NULL;
        callback->Size = NULL;
        callback->IsAwait = IsAwait;
        callback->Status = KBUSY;
        callback->Handler = &Srv_Pci_FindDevice_Callback; 

        struct ShareDataWithArguments_t data;
        data.Data = (uintptr_t)SearchParameters;
        data.Size = sizeof(srv_pci_search_parameters_t);
        data.ParameterPosition = 0x2;

        struct kot_arguments_t parameters;
        parameters.arg[0] = SrvPciCallbackThread;
        parameters.arg[1] = (uint64_t)callback;
        // arg  2 is reserved for srv_pci_search_parameters_t
        parameters.arg[3] = Index;

        KResult Status = Sys_ExecThread(PciData->FindDevice, &parameters, ExecutionTypeQueu, &data);
        if(Status == KSUCCESS && IsAwait){
            Sys_Pause(false);
        }
        return callback;
    }

    /* GetInfoDevice */
    KResult Srv_Pci_GetInfoDevice_Callback(KResult Status, struct srv_pci_callback_t* Callback, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3){
        if(Status == KSUCCESS){
            Callback->Data = (uint64_t)malloc(sizeof(srv_pci_device_info_t));
            memcpy((void*)Callback->Data, (void*)GP0, sizeof(srv_pci_device_info_t));
            Callback->Size = sizeof(srv_pci_device_info_t);
        }
        return Status;
    }

    struct srv_pci_callback_t* Srv_Pci_GetInfoDevice(PCIDeviceID_t Device, bool IsAwait){
        if(!SrvPciCallbackThread) Srv_Pci_Initialize();
        uisd_pci_t* PciData = (uisd_pci_t*)FindControllerUISD(ControllerTypeEnum_PCI);

        kot_thread_t self = Sys_GetThread();

        struct srv_pci_callback_t* callback = (struct srv_pci_callback_t*)malloc(sizeof(struct srv_pci_callback_t));
        callback->Self = self;
        callback->Data = NULL;
        callback->Size = NULL;
        callback->IsAwait = IsAwait;
        callback->Status = KBUSY;
        callback->Handler = &Srv_Pci_GetInfoDevice_Callback; 

        struct kot_arguments_t parameters;
        parameters.arg[0] = SrvPciCallbackThread;
        parameters.arg[1] = (uint64_t)callback;
        parameters.arg[2] = Device;

        KResult Status = Sys_ExecThread(PciData->GetInfoDevice, &parameters, ExecutionTypeQueu, NULL);
        if(Status == KSUCCESS && IsAwait){
            Sys_Pause(false);
        }
        return callback;
    }

    /* GetBAR */
    KResult Srv_Pci_GetBAR_Callback(KResult Status, struct srv_pci_callback_t* Callback, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3){
        if(Status == KSUCCESS){
            Callback->Data = (uint64_t)malloc(sizeof(srv_pci_bar_info_t));
            memcpy((void*)Callback->Data, (void*)GP0, sizeof(srv_pci_bar_info_t));
            Callback->Size = sizeof(srv_pci_bar_info_t);
        }
        return Status;
    }

    struct srv_pci_callback_t* Srv_Pci_GetBAR(PCIDeviceID_t Device, uint8_t BarIndex, bool IsAwait){
        if(!SrvPciCallbackThread) Srv_Pci_Initialize();
        uisd_pci_t* PciData = (uisd_pci_t*)FindControllerUISD(ControllerTypeEnum_PCI);

        kot_thread_t self = Sys_GetThread();

        struct srv_pci_callback_t* callback = (struct srv_pci_callback_t*)malloc(sizeof(struct srv_pci_callback_t));
        callback->Self = self;
        callback->Data = NULL;
        callback->Size = NULL;
        callback->IsAwait = IsAwait;
        callback->Status = KBUSY;
        callback->Handler = &Srv_Pci_GetBAR_Callback; 

        struct kot_arguments_t parameters;
        parameters.arg[0] = SrvPciCallbackThread;
        parameters.arg[1] = (uint64_t)callback;
        parameters.arg[2] = Device;
        parameters.arg[3] = BarIndex;

        KResult Status = Sys_ExecThread(PciData->GetBARDevice, &parameters, ExecutionTypeQueu, NULL);

        if(Status == KSUCCESS && IsAwait){
            Sys_Pause(false);
        }
        return callback;
    }

    /* BindMSI */
    KResult Srv_Pci_BindMSI_Callback(KResult Status, struct srv_pci_callback_t* Callback, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3){
        if(Status == KSUCCESS){
            Callback->Data = GP0;
        }
        return Status;
    }

    struct srv_pci_callback_t* Srv_Pci_BindMSI(PCIDeviceID_t Device, uint8_t IRQVector, uint8_t Processor, uint16_t LocalDeviceVector, bool IsAwait){
        if(!SrvPciCallbackThread) Srv_Pci_Initialize();
        uisd_pci_t* PciData = (uisd_pci_t*)FindControllerUISD(ControllerTypeEnum_PCI);

        kot_thread_t self = Sys_GetThread();

        struct srv_pci_callback_t* callback = (struct srv_pci_callback_t*)malloc(sizeof(struct srv_pci_callback_t));
        callback->Self = self;
        callback->Data = NULL;
        callback->Size = NULL;
        callback->IsAwait = IsAwait;
        callback->Status = KBUSY;
        callback->Handler = &Srv_Pci_BindMSI_Callback; 

        struct kot_arguments_t parameters;
        parameters.arg[0] = SrvPciCallbackThread;
        parameters.arg[1] = (uint64_t)callback;
        parameters.arg[2] = Device;
        parameters.arg[3] = IRQVector;
        parameters.arg[4] = Processor;
        parameters.arg[5] = LocalDeviceVector;

        KResult Status = Sys_ExecThread(PciData->BindMSI, &parameters, ExecutionTypeQueu, NULL);

        if(Status == KSUCCESS && IsAwait){
            Sys_Pause(false);
        }
        return callback;
    }

    /* UnbindMSI */
    KResult Srv_Pci_UnbindMSI_Callback(KResult Status, struct srv_pci_callback_t* Callback, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3){
        return Status;
    }

    struct srv_pci_callback_t* Srv_Pci_UnbindMSI(PCIDeviceID_t Device, uint16_t LocalDeviceVector, bool IsAwait){
        if(!SrvPciCallbackThread) Srv_Pci_Initialize();
        uisd_pci_t* PciData = (uisd_pci_t*)FindControllerUISD(ControllerTypeEnum_PCI);

        kot_thread_t self = Sys_GetThread();

        struct srv_pci_callback_t* callback = (struct srv_pci_callback_t*)malloc(sizeof(struct srv_pci_callback_t));
        callback->Self = self;
        callback->Data = NULL;
        callback->Size = NULL;
        callback->IsAwait = IsAwait;
        callback->Status = KBUSY;
        callback->Handler = &Srv_Pci_UnbindMSI_Callback; 

        struct kot_arguments_t parameters;
        parameters.arg[0] = SrvPciCallbackThread;
        parameters.arg[1] = (uint64_t)callback;
        parameters.arg[2] = Device;
        parameters.arg[3] = LocalDeviceVector;

        KResult Status = Sys_ExecThread(PciData->UnbindMSI, &parameters, ExecutionTypeQueu, NULL);

        if(Status == KSUCCESS && IsAwait){
            Sys_Pause(false);
        }
        return callback;
    }

    /* ConfigReadWord */
    KResult Srv_Pci_ConfigReadWord_Callback(KResult Status, struct srv_pci_callback_t* Callback, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3){
        if(Status == KSUCCESS){
            Callback->Data = GP0;
            Callback->Size = sizeof(uint16_t);
        }
        return Status;
    }

    struct srv_pci_callback_t* Srv_Pci_ConfigReadWord(PCIDeviceID_t Device, uint16_t Offset, bool IsAwait){
        if(!SrvPciCallbackThread) Srv_Pci_Initialize();
        uisd_pci_t* PciData = (uisd_pci_t*)FindControllerUISD(ControllerTypeEnum_PCI);

        kot_thread_t self = Sys_GetThread();

        struct srv_pci_callback_t* callback = (struct srv_pci_callback_t*)malloc(sizeof(struct srv_pci_callback_t));
        callback->Self = self;
        callback->Data = NULL;
        callback->Size = NULL;
        callback->IsAwait = IsAwait;
        callback->Status = KBUSY;
        callback->Handler = &Srv_Pci_ConfigReadWord_Callback; 

        struct kot_arguments_t parameters;
        parameters.arg[0] = SrvPciCallbackThread;
        parameters.arg[1] = (uint64_t)callback;
        parameters.arg[2] = Device;
        parameters.arg[3] = Offset;

        KResult Status = Sys_ExecThread(PciData->ConfigReadWord, &parameters, ExecutionTypeQueu, NULL);

        if(Status == KSUCCESS && IsAwait){
            Sys_Pause(false);
        }
        return callback;
    }

    /* ConfigWriteWord */
    KResult Srv_Pci_ConfigWriteWord_Callback(KResult Status, struct srv_pci_callback_t* Callback, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3){
        return Status;
    }

    struct srv_pci_callback_t* Srv_Pci_ConfigWriteWord(PCIDeviceID_t Device, uint16_t Offset, uint16_t Value, bool IsAwait){
        if(!SrvPciCallbackThread) Srv_Pci_Initialize();
        uisd_pci_t* PciData = (uisd_pci_t*)FindControllerUISD(ControllerTypeEnum_PCI);

        kot_thread_t self = Sys_GetThread();

        struct srv_pci_callback_t* callback = (struct srv_pci_callback_t*)malloc(sizeof(struct srv_pci_callback_t));
        callback->Self = self;
        callback->Data = NULL;
        callback->Size = NULL;
        callback->IsAwait = IsAwait;
        callback->Status = KBUSY;
        callback->Handler = &Srv_Pci_ConfigWriteWord_Callback; 

        struct kot_arguments_t parameters;
        parameters.arg[0] = SrvPciCallbackThread;
        parameters.arg[1] = (uint64_t)callback;
        parameters.arg[2] = Device;
        parameters.arg[3] = Offset;
        parameters.arg[4] = Value;

        KResult Status = Sys_ExecThread(PciData->ConfigWriteWord, &parameters, ExecutionTypeQueu, NULL);

        if(Status == KSUCCESS && IsAwait){
            Sys_Pause(false);
        }
        return callback;
    }
}