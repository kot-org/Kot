#include <srv/srv.h>

PCIDeviceArrayInfo_t* SrvDevicesArray;

void InitSrv(PCIDeviceArrayInfo_t* DevicesArray){
    SrvDevicesArray = DevicesArray;

    uintptr_t addr = GetFreeAlignedSpace(sizeof(uisd_pci_t));
    ksmem_t key = NULL;
    
    process_t proc = Sys_GetProcess();

    Sys_CreateMemoryField(proc, sizeof(uisd_pci_t), &addr, &key, MemoryFieldTypeShareSpaceRO);

    uisd_pci_t* PciSrv = (uisd_pci_t*) addr;
    PciSrv->ControllerHeader.IsReadWrite = false;
    PciSrv->ControllerHeader.Version = Pci_Srv_Version;
    PciSrv->ControllerHeader.VendorID = Kot_VendorID;
    PciSrv->ControllerHeader.Type = ControllerTypeEnum_PCI;
    PciSrv->ControllerHeader.Process = ShareProcessKey(proc);

    /* Setup thread */
    CreateControllerUISD(ControllerTypeEnum_PCI, key, true);
    
    /* CountDevices */
    thread_t CountDevicesThread = NULL;
    Sys_CreateThread(proc, (uintptr_t) &CountDevices, PriviledgeDriver, NULL, &CountDevicesThread);
    PciSrv->CountDevices = MakeShareableThread(CountDevicesThread, PriviledgeDriver);

    /* FindDevice */
    thread_t FindDeviceThread = NULL;
    Sys_CreateThread(proc, (uintptr_t) &FindDevice, PriviledgeDriver, NULL, &FindDeviceThread);
    PciSrv->FindDevice = MakeShareableThread(FindDeviceThread, PriviledgeDriver);

    /* GetInfo */
    thread_t GetInfoDeviceThread = NULL;
    Sys_CreateThread(proc, (uintptr_t) &GetInfoDevice, PriviledgeDriver, NULL, &GetInfoDeviceThread);
    PciSrv->GetInfoDevice = MakeShareableThread(GetInfoDeviceThread, PriviledgeDriver);

    /* GetBAR */
    thread_t GetBARDeviceThread = NULL;
    Sys_CreateThread(proc, (uintptr_t) &GetBARDevice, PriviledgeDriver, NULL, &GetBARDeviceThread);
    PciSrv->GetBARDevice = MakeShareableThread(GetBARDeviceThread, PriviledgeDriver);

    /* BindMSI */
    thread_t BindMSIThread = NULL;
    Sys_CreateThread(proc, (uintptr_t) &BindMSI, PriviledgeDriver, NULL, &BindMSIThread);
    PciSrv->BindMSI = MakeShareableThread(BindMSIThread, PriviledgeDriver);

    /* UnbindMSI */
    thread_t UnbindMSIThread = NULL;
    Sys_CreateThread(proc, (uintptr_t) &UnbindMSI, PriviledgeDriver, NULL, &UnbindMSIThread);
    PciSrv->UnbindMSI = MakeShareableThread(UnbindMSIThread, PriviledgeDriver);

    /* ConfigReadWord */
    thread_t ConfigReadWordThread = NULL;
    Sys_CreateThread(proc, (uintptr_t) &ConfigReadWord, PriviledgeDriver, NULL, &ConfigReadWordThread);
    PciSrv->ConfigReadWord = MakeShareableThread(ConfigReadWordThread, PriviledgeDriver);

    /* ConfigWriteWord */
    thread_t ConfigWriteWordThread = NULL;
    Sys_CreateThread(proc, (uintptr_t) &ConfigWriteWord, PriviledgeDriver, NULL, &ConfigWriteWordThread);
    PciSrv->ConfigWriteWord = MakeShareableThread(ConfigWriteWordThread, PriviledgeDriver);
}

KResult CountDevices(thread_t Callback, uint64_t CallbackArg, srv_pci_search_parameters_t* SearchParameters){
    uint64_t NumDeviceFound = Search(SrvDevicesArray, SearchParameters->vendorID, SearchParameters->deviceID, SearchParameters->classID, SearchParameters->subClassID, SearchParameters->progIF);
    
    arguments_t arguments{
        .arg[0] = KSUCCESS,           /* Status */
        .arg[1] = CallbackArg,      /* CallbackArg */
        .arg[2] = NumDeviceFound,   /* NumDeviceFound */
        .arg[3] = NULL,             /* GP1 */
        .arg[4] = NULL,             /* GP2 */
        .arg[5] = NULL,             /* GP3 */
    };

    Sys_ExecThread(Callback, &arguments, ExecutionTypeQueu, NULL);
    Sys_Close(KSUCCESS);
}

KResult FindDevice(thread_t Callback, uint64_t CallbackArg, srv_pci_search_parameters_t* SearchParameters, uint64_t Index){
    PCIDeviceID_t Device = NULL;
    Device = GetDevice(SrvDevicesArray, SearchParameters->vendorID, SearchParameters->deviceID, SearchParameters->classID, SearchParameters->subClassID, SearchParameters->progIF, Index);

    arguments_t arguments{
        .arg[0] = KSUCCESS,         /* Status */
        .arg[1] = CallbackArg,      /* CallbackArg */
        .arg[2] = Device,           /* Device */
        .arg[3] = NULL,             /* GP1 */
        .arg[4] = NULL,             /* GP2 */
        .arg[5] = NULL,             /* GP3 */
    };

    Sys_ExecThread(Callback, &arguments, ExecutionTypeQueu, NULL);
    Sys_Close(KSUCCESS);    
}

KResult GetInfoDevice(thread_t Callback, uint64_t CallbackArg, PCIDeviceID_t DeviceIndex){
    KResult Status = KFAIL;
    srv_pci_device_info_t DeviceInfo;
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

    ShareDataWithArguments_t data{
        .Data = &DeviceInfo,
        .Size = sizeof(srv_pci_device_info_t),
        .ParameterPosition = 0x2,
    };

    arguments_t arguments{
        .arg[0] = Status,           /* Status */
        .arg[1] = CallbackArg,      /* CallbackArg */
        .arg[2] = NULL,             /* Data */
        .arg[3] = NULL,             /* GP1 */
        .arg[4] = NULL,             /* GP2 */
        .arg[5] = NULL,             /* GP3 */
    };

    Sys_ExecThread(Callback, &arguments, ExecutionTypeQueu, &data);
    Sys_Close(KSUCCESS);
}

KResult GetBARDevice(thread_t Callback, uint64_t CallbackArg, PCIDeviceID_t DeviceIndex, uint8_t BarIndex){
    KResult Status = KFAIL;
    srv_pci_bar_info_t BARInfo;
    if(CheckDevice(SrvDevicesArray, DeviceIndex)){
        PCIDevice_t* Device = GetDeviceFromIndex(SrvDevicesArray, DeviceIndex);
        BARInfo.Address = Device->GetBarAddress(BarIndex);
        BARInfo.Size = Device->GetBarSize(BarIndex);
        BARInfo.Type = Device->GetBarType(BarIndex);
        Status = KSUCCESS;
    }

    ShareDataWithArguments_t data{
        .Data = &BARInfo,
        .Size = sizeof(srv_pci_bar_info_t),
        .ParameterPosition = 0x2,
    };

    arguments_t arguments{
        .arg[0] = Status,           /* Status */
        .arg[1] = CallbackArg,      /* CallbackArg */
        .arg[2] = NULL,             /* Data */
        .arg[3] = NULL,             /* GP1 */
        .arg[4] = NULL,             /* GP2 */
        .arg[5] = NULL,             /* GP3 */
    };

    Sys_ExecThread(Callback, &arguments, ExecutionTypeQueu, &data);
    Sys_Close(KSUCCESS);
}

KResult BindMSI(thread_t Callback, uint64_t CallbackArg, PCIDeviceID_t DeviceIndex, uint8_t IRQVector, uint8_t Processor, uint16_t LocalDeviceVector){
    KResult Status = KFAIL;
    srv_pci_bar_info_t BARInfo;
    uint64_t Version = 0;
    if(CheckDevice(SrvDevicesArray, DeviceIndex)){
        PCIDevice_t* Device = GetDeviceFromIndex(SrvDevicesArray, DeviceIndex);
        Status = Device->BindMSI(IRQVector, Processor, LocalDeviceVector,&Version);
    }

    arguments_t arguments{
        .arg[0] = Status,           /* Status */
        .arg[1] = CallbackArg,      /* CallbackArg */
        .arg[2] = Version,          /* Version */
        .arg[3] = NULL,             /* GP1 */
        .arg[4] = NULL,             /* GP2 */
        .arg[5] = NULL,             /* GP3 */
    };

    Sys_ExecThread(Callback, &arguments, ExecutionTypeQueu, NULL);
    Sys_Close(KSUCCESS);
}

KResult UnbindMSI(thread_t Callback, uint64_t CallbackArg, PCIDeviceID_t DeviceIndex, uint16_t LocalDeviceVector){
    KResult Status = KFAIL;
    if(CheckDevice(SrvDevicesArray, DeviceIndex)){
        PCIDevice_t* Device = GetDeviceFromIndex(SrvDevicesArray, DeviceIndex);
        Status = Device->UnbindMSI(LocalDeviceVector);
    }

    arguments_t arguments{
        .arg[0] = Status,           /* Status */
        .arg[1] = CallbackArg,      /* CallbackArg */
        .arg[2] = NULL,             /* GP0 */
        .arg[3] = NULL,             /* GP1 */
        .arg[4] = NULL,             /* GP2 */
        .arg[5] = NULL,             /* GP3 */
    };

    Sys_ExecThread(Callback, &arguments, ExecutionTypeQueu, NULL);
    Sys_Close(KSUCCESS);
}

KResult ConfigReadWord(thread_t Callback, uint64_t CallbackArg, PCIDeviceID_t DeviceIndex, uint16_t Offset){
    KResult Status = KFAIL;
    uint16_t Value;
    if(CheckDevice(SrvDevicesArray, DeviceIndex)){
        PCIDevice_t* Device = GetDeviceFromIndex(SrvDevicesArray, DeviceIndex);
        Status = Device->ConfigReadWord(Offset, &Value);
    }

    arguments_t arguments{
        .arg[0] = Status,           /* Status */
        .arg[1] = CallbackArg,      /* CallbackArg */
        .arg[2] = Value,            /* Value */
        .arg[3] = NULL,             /* GP1 */
        .arg[4] = NULL,             /* GP2 */
        .arg[5] = NULL,             /* GP3 */
    };

    Sys_ExecThread(Callback, &arguments, ExecutionTypeQueu, NULL);
    Sys_Close(KSUCCESS);
}

KResult ConfigWriteWord(thread_t Callback, uint64_t CallbackArg, PCIDeviceID_t DeviceIndex, uint16_t Offset, uint16_t Value){
    KResult Status = KFAIL;
    if(CheckDevice(SrvDevicesArray, DeviceIndex)){
        PCIDevice_t* Device = GetDeviceFromIndex(SrvDevicesArray, DeviceIndex);
        Status = Device->ConfigWriteWord(Offset, Value);
    }

    arguments_t arguments{
        .arg[0] = Status,           /* Status */
        .arg[1] = CallbackArg,      /* CallbackArg */
        .arg[2] = NULL,             /* GP0 */
        .arg[3] = NULL,             /* GP1 */
        .arg[4] = NULL,             /* GP2 */
        .arg[5] = NULL,             /* GP3 */
    };

    Sys_ExecThread(Callback, &arguments, ExecutionTypeQueu, NULL);
    Sys_Close(KSUCCESS);
}