#include <srv/srv.h>

PCIDeviceArrayInfo_t* SrvDevicesArray;

void InitSrv(PCIDeviceArrayInfo_t* DevicesArray){
    SrvDevicesArray = DevicesArray;

    void* addr = kot_GetFreeAlignedSpace(sizeof(kot_uisd_pci_t));
    kot_key_mem_t key = NULL;
    
    kot_process_t proc = kot_Sys_GetProcess();

    kot_Sys_CreateMemoryField(proc, sizeof(kot_uisd_pci_t), &addr, &key, MemoryFieldTypeShareSpaceRO);

    kot_uisd_pci_t* PciSrv = (kot_uisd_pci_t*) addr;
    PciSrv->ControllerHeader.IsReadWrite = false;
    PciSrv->ControllerHeader.Version = Pci_Srv_Version;
    PciSrv->ControllerHeader.VendorID = Kot_VendorID;
    PciSrv->ControllerHeader.Type = ControllerTypeEnum_PCI;
    PciSrv->ControllerHeader.Process = kot_ShareProcessKey(proc);

    /* Setup thread */
    kot_CreateControllerUISD(ControllerTypeEnum_PCI, key, true);
    
    /* CountDevices */
    kot_thread_t CountDevicesThread = NULL;
    kot_Sys_CreateThread(proc, (void*) &CountDevices, PriviledgeDriver, NULL, &CountDevicesThread);
    PciSrv->CountDevices = kot_MakeShareableThread(CountDevicesThread, PriviledgeDriver);

    /* FindDevice */
    kot_thread_t FindDeviceThread = NULL;
    kot_Sys_CreateThread(proc, (void*) &FindDevice, PriviledgeDriver, NULL, &FindDeviceThread);
    PciSrv->FindDevice = kot_MakeShareableThread(FindDeviceThread, PriviledgeDriver);

    /* GetInfo */
    kot_thread_t GetInfoDeviceThread = NULL;
    kot_Sys_CreateThread(proc, (void*) &GetInfoDevice, PriviledgeDriver, NULL, &GetInfoDeviceThread);
    PciSrv->GetInfoDevice = kot_MakeShareableThread(GetInfoDeviceThread, PriviledgeDriver);

    /* GetBAR */
    kot_thread_t GetBARDeviceThread = NULL;
    kot_Sys_CreateThread(proc, (void*) &GetBARDevice, PriviledgeDriver, NULL, &GetBARDeviceThread);
    PciSrv->GetBARDevice = kot_MakeShareableThread(GetBARDeviceThread, PriviledgeDriver);

    /* BindMSI */
    kot_thread_t BindMSIThread = NULL;
    kot_Sys_CreateThread(proc, (void*) &BindMSI, PriviledgeDriver, NULL, &BindMSIThread);
    PciSrv->BindMSI = kot_MakeShareableThread(BindMSIThread, PriviledgeDriver);

    /* UnbindMSI */
    kot_thread_t UnbindMSIThread = NULL;
    kot_Sys_CreateThread(proc, (void*) &UnbindMSI, PriviledgeDriver, NULL, &UnbindMSIThread);
    PciSrv->UnbindMSI = kot_MakeShareableThread(UnbindMSIThread, PriviledgeDriver);

    /* ConfigReadWord */
    kot_thread_t ConfigReadWordThread = NULL;
    kot_Sys_CreateThread(proc, (void*) &ConfigReadWord, PriviledgeDriver, NULL, &ConfigReadWordThread);
    PciSrv->ConfigReadWord = kot_MakeShareableThread(ConfigReadWordThread, PriviledgeDriver);

    /* ConfigWriteWord */
    kot_thread_t ConfigWriteWordThread = NULL;
    kot_Sys_CreateThread(proc, (void*) &ConfigWriteWord, PriviledgeDriver, NULL, &ConfigWriteWordThread);
    PciSrv->ConfigWriteWord = kot_MakeShareableThread(ConfigWriteWordThread, PriviledgeDriver);
}

KResult CountDevices(kot_thread_t Callback, uint64_t CallbackArg, kot_srv_pci_search_parameters_t* SearchParameters){
    uint64_t NumDeviceFound = Search(SrvDevicesArray, SearchParameters->vendorID, SearchParameters->deviceID, SearchParameters->classID, SearchParameters->subClassID, SearchParameters->progIF);
    
    kot_arguments_t arguments{
        .arg[0] = KSUCCESS,           /* Status */
        .arg[1] = CallbackArg,      /* CallbackArg */
        .arg[2] = NumDeviceFound,   /* NumDeviceFound */
        .arg[3] = NULL,             /* GP1 */
        .arg[4] = NULL,             /* GP2 */
        .arg[5] = NULL,             /* GP3 */
    };

    kot_Sys_ExecThread(Callback, &arguments, ExecutionTypeQueu, NULL);
    kot_Sys_Close(KSUCCESS);
}

KResult FindDevice(kot_thread_t Callback, uint64_t CallbackArg, kot_srv_pci_search_parameters_t* SearchParameters, uint64_t Index){
    kot_PCIDeviceID_t Device = NULL;
    Device = GetDevice(SrvDevicesArray, SearchParameters->vendorID, SearchParameters->deviceID, SearchParameters->classID, SearchParameters->subClassID, SearchParameters->progIF, Index);

    kot_arguments_t arguments{
        .arg[0] = KSUCCESS,         /* Status */
        .arg[1] = CallbackArg,      /* CallbackArg */
        .arg[2] = Device,           /* Device */
        .arg[3] = NULL,             /* GP1 */
        .arg[4] = NULL,             /* GP2 */
        .arg[5] = NULL,             /* GP3 */
    };

    kot_Sys_ExecThread(Callback, &arguments, ExecutionTypeQueu, NULL);
    kot_Sys_Close(KSUCCESS);    
}

KResult GetInfoDevice(kot_thread_t Callback, uint64_t CallbackArg, kot_PCIDeviceID_t DeviceIndex){
    KResult Status = KFAIL;
    kot_srv_pci_device_info_t DeviceInfo;
    if(CheckDevice(SrvDevicesArray, DeviceIndex)){
        PCIDevice_t* Device = GetDeviceFromIndex(SrvDevicesArray, DeviceIndex);
        PCIDeviceHeader_t* Header = (PCIDeviceHeader_t*)Device->ConfigurationSpace;
        DeviceInfo.vendorID = Header->VendorID;
        DeviceInfo.deviceID = Header->DeviceID;
        DeviceInfo.classID = Header->Class;
        DeviceInfo.subClassID = Header->Subclass;
        DeviceInfo.progIF = Header->ProgIF;
        Status = KSUCCESS;
    }

    kot_ShareDataWithArguments_t data{
        .Data = &DeviceInfo,
        .Size = sizeof(kot_srv_pci_device_info_t),
        .ParameterPosition = 0x2,
    };

    kot_arguments_t arguments{
        .arg[0] = Status,           /* Status */
        .arg[1] = CallbackArg,      /* CallbackArg */
        .arg[2] = NULL,             /* Data */
        .arg[3] = NULL,             /* GP1 */
        .arg[4] = NULL,             /* GP2 */
        .arg[5] = NULL,             /* GP3 */
    };

    kot_Sys_ExecThread(Callback, &arguments, ExecutionTypeQueu, &data);
    kot_Sys_Close(KSUCCESS);
}

KResult GetBARDevice(kot_thread_t Callback, uint64_t CallbackArg, kot_PCIDeviceID_t DeviceIndex, uint8_t BarIndex){
    KResult Status = KFAIL;
    kot_srv_pci_bar_info_t BARInfo;
    if(CheckDevice(SrvDevicesArray, DeviceIndex)){
        PCIDevice_t* Device = GetDeviceFromIndex(SrvDevicesArray, DeviceIndex);
        BARInfo.Address = Device->GetBarAddress(BarIndex);
        BARInfo.Size = Device->GetBarSize(BarIndex);
        BARInfo.Type = Device->GetBarType(BarIndex);
        Status = KSUCCESS;
    }

    kot_ShareDataWithArguments_t data{
        .Data = &BARInfo,
        .Size = sizeof(kot_srv_pci_bar_info_t),
        .ParameterPosition = 0x2,
    };

    kot_arguments_t arguments{
        .arg[0] = Status,           /* Status */
        .arg[1] = CallbackArg,      /* CallbackArg */
        .arg[2] = NULL,             /* Data */
        .arg[3] = NULL,             /* GP1 */
        .arg[4] = NULL,             /* GP2 */
        .arg[5] = NULL,             /* GP3 */
    };

    kot_Sys_ExecThread(Callback, &arguments, ExecutionTypeQueu, &data);
    kot_Sys_Close(KSUCCESS);
}

KResult BindMSI(kot_thread_t Callback, uint64_t CallbackArg, kot_PCIDeviceID_t DeviceIndex, uint8_t IRQVector, uint8_t Processor, uint16_t LocalDeviceVector){
    KResult Status = KFAIL;
    kot_srv_pci_bar_info_t BARInfo;
    uint64_t Version = 0;
    if(CheckDevice(SrvDevicesArray, DeviceIndex)){
        PCIDevice_t* Device = GetDeviceFromIndex(SrvDevicesArray, DeviceIndex);
        Status = Device->BindMSI(IRQVector, Processor, LocalDeviceVector,&Version);
    }

    kot_arguments_t arguments{
        .arg[0] = Status,           /* Status */
        .arg[1] = CallbackArg,      /* CallbackArg */
        .arg[2] = Version,          /* Version */
        .arg[3] = NULL,             /* GP1 */
        .arg[4] = NULL,             /* GP2 */
        .arg[5] = NULL,             /* GP3 */
    };

    kot_Sys_ExecThread(Callback, &arguments, ExecutionTypeQueu, NULL);
    kot_Sys_Close(KSUCCESS);
}

KResult UnbindMSI(kot_thread_t Callback, uint64_t CallbackArg, kot_PCIDeviceID_t DeviceIndex, uint16_t LocalDeviceVector){
    KResult Status = KFAIL;
    if(CheckDevice(SrvDevicesArray, DeviceIndex)){
        PCIDevice_t* Device = GetDeviceFromIndex(SrvDevicesArray, DeviceIndex);
        Status = Device->UnbindMSI(LocalDeviceVector);
    }

    kot_arguments_t arguments{
        .arg[0] = Status,           /* Status */
        .arg[1] = CallbackArg,      /* CallbackArg */
        .arg[2] = NULL,             /* GP0 */
        .arg[3] = NULL,             /* GP1 */
        .arg[4] = NULL,             /* GP2 */
        .arg[5] = NULL,             /* GP3 */
    };

    kot_Sys_ExecThread(Callback, &arguments, ExecutionTypeQueu, NULL);
    kot_Sys_Close(KSUCCESS);
}

KResult ConfigReadWord(kot_thread_t Callback, uint64_t CallbackArg, kot_PCIDeviceID_t DeviceIndex, uint16_t Offset){
    KResult Status = KFAIL;
    uint16_t Value;
    if(CheckDevice(SrvDevicesArray, DeviceIndex)){
        PCIDevice_t* Device = GetDeviceFromIndex(SrvDevicesArray, DeviceIndex);
        Status = Device->ConfigReadWord(Offset, &Value);
    }

    kot_arguments_t arguments{
        .arg[0] = Status,           /* Status */
        .arg[1] = CallbackArg,      /* CallbackArg */
        .arg[2] = Value,            /* Value */
        .arg[3] = NULL,             /* GP1 */
        .arg[4] = NULL,             /* GP2 */
        .arg[5] = NULL,             /* GP3 */
    };

    kot_Sys_ExecThread(Callback, &arguments, ExecutionTypeQueu, NULL);
    kot_Sys_Close(KSUCCESS);
}

KResult ConfigWriteWord(kot_thread_t Callback, uint64_t CallbackArg, kot_PCIDeviceID_t DeviceIndex, uint16_t Offset, uint16_t Value){
    KResult Status = KFAIL;
    if(CheckDevice(SrvDevicesArray, DeviceIndex)){
        PCIDevice_t* Device = GetDeviceFromIndex(SrvDevicesArray, DeviceIndex);
        Status = Device->ConfigWriteWord(Offset, Value);
    }

    kot_arguments_t arguments{
        .arg[0] = Status,           /* Status */
        .arg[1] = CallbackArg,      /* CallbackArg */
        .arg[2] = NULL,             /* GP0 */
        .arg[3] = NULL,             /* GP1 */
        .arg[4] = NULL,             /* GP2 */
        .arg[5] = NULL,             /* GP3 */
    };

    kot_Sys_ExecThread(Callback, &arguments, ExecutionTypeQueu, NULL);
    kot_Sys_Close(KSUCCESS);
}