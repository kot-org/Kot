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
    
    /* PCISearcher */
    thread_t PCISearcherThread = NULL;
    Sys_Createthread(proc, (uintptr_t) &PCISearcher, PriviledgeDriver, &PCISearcherThread);
    PciSrv->PCISearcher = MakeShareableThread(PCISearcherThread, PriviledgeDriver);

    /* PCIGetDevice */
    thread_t PCIGetDeviceThread = NULL;
    Sys_Createthread(proc, (uintptr_t) &PCIGetDevice, PriviledgeDriver, &PCIGetDeviceThread);
    PciSrv->PCIGetDevice = MakeShareableThread(PCIGetDeviceThread, PriviledgeDriver);

    /* GetInfo */
    thread_t GetInfoThread = NULL;
    Sys_Createthread(proc, (uintptr_t) &GetInfo, PriviledgeDriver, &GetInfoThread);
    PciSrv->GetInfo = MakeShareableThread(GetInfoThread, PriviledgeDriver);

    /* GetBAR */
    thread_t GetBARThread = NULL;
    Sys_Createthread(proc, (uintptr_t) &GetBAR, PriviledgeDriver, &GetBARThread);
    PciSrv->GetBAR = MakeShareableThread(GetBARThread, PriviledgeDriver);
}

KResult PCISearcher(thread_t Callback, uint64_t CallbackArg, srv_pci_search_parameters_t* SearchParameters){
    KResult Status = KFAIL;
    uint64_t NumDeviceFound = Search(SrvDevicesArray, SearchParameters->vendorID, SearchParameters->deviceID, SearchParameters->classID, SearchParameters->subClassID, SearchParameters->progIF);
    
    arguments_t arguments{
        .arg[0] = Status,           /* Status */
        .arg[1] = CallbackArg,      /* CallbackArg */
        .arg[2] = NumDeviceFound,   /* NumDeviceFound */
        .arg[3] = NULL,             /* GP1 */
        .arg[4] = NULL,             /* GP2 */
        .arg[5] = NULL,             /* GP3 */
    };

    Sys_Execthread(Callback, &arguments, ExecutionTypeQueu, NULL);
    Sys_Close(KSUCCESS);
}

KResult PCIGetDevice(thread_t Callback, uint64_t CallbackArg, srv_pci_search_parameters_t* SearchParameters, uint64_t Index){
    KResult Status = KFAIL;
    PCIDeviceID_t Device = NULL;
    Device = GetDevice(SrvDevicesArray, SearchParameters->vendorID, SearchParameters->deviceID, SearchParameters->classID, SearchParameters->subClassID, SearchParameters->progIF, Index);

    arguments_t arguments{
        .arg[0] = Status,           /* Status */
        .arg[1] = CallbackArg,      /* CallbackArg */
        .arg[2] = Device,           /* Device */
        .arg[3] = NULL,             /* GP1 */
        .arg[4] = NULL,             /* GP2 */
        .arg[5] = NULL,             /* GP3 */
    };

    Sys_Execthread(Callback, &arguments, ExecutionTypeQueu, NULL);
    Sys_Close(KSUCCESS);    
}

KResult GetInfo(thread_t Callback, uint64_t CallbackArg, PCIDeviceID_t DeviceIndex){
    KResult Status = KFAIL;
    uint64_t VendorID = NULL;
    uint64_t DeviceID = NULL;
    uint64_t Class = NULL;
    uint64_t Subclass = NULL;
    if(CheckDevice(SrvDevicesArray, DeviceIndex)){
        PCIDevice_t* Device = GetDevice(SrvDevicesArray, DeviceIndex);
        PCIDeviceHeader_t* Header = (PCIDeviceHeader_t*)Device->ConfigurationSpace;
        VendorID = Header->VendorID;
        DeviceID = Header->DeviceID;
        Class = Header->Class;
        Subclass = Header->Subclass;
    }

    arguments_t arguments{
        .arg[0] = Status,           /* Status */
        .arg[1] = CallbackArg,      /* CallbackArg */
        .arg[2] = VendorID,         /* VendorID */
        .arg[3] = DeviceID,         /* DeviceID */
        .arg[4] = Class,            /* Class */
        .arg[5] = Subclass,         /* Subclass */
    };

    Sys_Execthread(Callback, &arguments, ExecutionTypeQueu, NULL);
    Sys_Close(KSUCCESS);
}

KResult GetBAR(thread_t Callback, uint64_t CallbackArg, PCIDeviceID_t DeviceIndex, uint8_t BarIndex){
    KResult Status = KFAIL;
    uint64_t BARAddress = NULL;
    uint64_t BARSize = NULL;
    uint64_t BARType = NULL;
    if(CheckDevice(SrvDevicesArray, DeviceIndex)){
        PCIDevice_t* Device = GetDevice(SrvDevicesArray, DeviceIndex);
        BARAddress = (uint64_t)Device->GetBarAddress(BarIndex);
        BARSize = Device->GetBarSize(BarIndex);
        BARType = Device->GetBarType(BarIndex);
        Status = KSUCCESS;
    }

    arguments_t arguments{
        .arg[0] = Status,           /* Status */
        .arg[1] = CallbackArg,      /* CallbackArg */
        .arg[2] = BARAddress,       /* BARAddress */
        .arg[3] = BARSize,          /* BARSize */
        .arg[4] = BARType,          /* BARType */
        .arg[5] = NULL,             /* GP3 */
    };

    Sys_Execthread(Callback, &arguments, ExecutionTypeQueu, NULL);
    Sys_Close(KSUCCESS);
}