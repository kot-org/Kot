#include <srv/srv.h>

PCIDeviceArrayInfo_t* SrvDevicesArray;

void InitSrv(PCIDeviceArrayInfo_t* DevicesArray){
    SrvDevicesArray = DevicesArray;

    uintptr_t addr = getFreeAlignedSpace(sizeof(uisd_pci_t));
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
    Sys_Createthread(proc, (uintptr_t) &CountDevices, PriviledgeDriver, &CountDevicesThread);
    PciSrv->CountDevices = MakeShareableThread(CountDevicesThread, PriviledgeDriver);

    /* FindDevice */
    thread_t FindDeviceThread = NULL;
    Sys_Createthread(proc, (uintptr_t) &FindDevice, PriviledgeDriver, &FindDeviceThread);
    PciSrv->FindDevice = MakeShareableThread(FindDeviceThread, PriviledgeDriver);

    /* GetInfo */
    thread_t GetInfoDeviceThread = NULL;
    Sys_Createthread(proc, (uintptr_t) &GetInfoDevice, PriviledgeDriver, &GetInfoDeviceThread);
    PciSrv->GetInfoDevice = MakeShareableThread(GetInfoDeviceThread, PriviledgeDriver);

    /* GetBAR */
    thread_t GetBARDeviceThread = NULL;
    Sys_Createthread(proc, (uintptr_t) &GetBARDevice, PriviledgeDriver, &GetBARDeviceThread);
    PciSrv->GetBARDevice = MakeShareableThread(GetBARDeviceThread, PriviledgeDriver);
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

    Sys_Execthread(Callback, &arguments, ExecutionTypeQueu, NULL);
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

    Sys_Execthread(Callback, &arguments, ExecutionTypeQueu, NULL);
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

    Sys_Execthread(Callback, &arguments, ExecutionTypeQueu, &data);
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

    Sys_Execthread(Callback, &arguments, ExecutionTypeQueu, &data);
    Sys_Close(KSUCCESS);
}