#include <srv/srv.h>

void InitSrv(){
    uintptr_t addr = getFreeAlignedSpace(sizeof(uisd_pci_t));
    ksmem_t key = NULL;
    
    proc = Sys_GetProcess();

    Sys_CreateMemoryField(proc, sizeof(uisd_pci_t), &addr, &key, MemoryFieldTypeShareSpaceRO);

    uisd_pci_t* PciSrv = (uisd_pci_t*) addr;
    PciSrv->ControllerHeader.IsReadWrite = false;
    PciSrv->ControllerHeader.Version = Pci_Srv_Version;
    PciSrv->ControllerHeader.VendorID = Kot_VendorID;
    PciSrv->ControllerHeader.Type = ControllerTypeEnum_PCI;
    PciSrv->ControllerHeader.Process = ShareProcessKey(proc);

    /* Setup thread */
    CreateControllerUISD(ControllerTypeEnum_PCI, key, true);

    /* GetBARNum */
    thread_t GetBARNumThread = NULL;
    Sys_Createthread(proc, (uintptr_t) &GetBARNum, PriviledgeDriver, &GetBARNumThread);
    PciSrv->GetBARNum = MakeShareableThread(GetBARNumThread, PriviledgeDriver);

    /* GetBARType */
    thread_t GetBARTypeThread = NULL;
    Sys_Createthread(proc, (uintptr_t) &GetBARType, PriviledgeDriver, &GetBARTypeThread);
    PciSrv->GetBARType = MakeShareableThread(GetBARTypeThread, PriviledgeDriver);

    /* GetBARSize */
    thread_t GetBARSizeThread = NULL;
    Sys_Createthread(proc, (uintptr_t) &GetBARSize, PriviledgeDriver, &GetBARSizeThread);
    PciSrv->GetBARSize = MakeShareableThread(GetBARSizeThread, PriviledgeDriver);
    
    /* PCISearcher */
    thread_t PCISearcherThread = NULL;
    Sys_Createthread(proc, (uintptr_t) &PCISearcher, PriviledgeDriver, &PCISearcherThread);
    PciSrv->PCISearcher = MakeShareableThread(PCISearcherThread, PriviledgeDriver);

    /* PCISearcherGetDevice */
    thread_t PCISearcherGetDeviceThread = NULL;
    Sys_Createthread(proc, (uintptr_t) &PCISearcherGetDevice, PriviledgeDriver, &PCISearcherGetDeviceThread);
    PciSrv->PCISearcherGetDevice = MakeShareableThread(PCISearcherGetDeviceThread, PriviledgeDriver);
}

KResult GetBARNum(thread_t Callback, uint64_t CallbackArg, PCIDeviceID_t Device){
    KResult Status = KFAIL;
    uint64_t BARNum = NULL;
    if(CheckDevice(Device)){
        BARNum = GetDevice(Device)->BARNum;
        Status = KSUCCESS;
    }

    arguments_t arguments{
        .arg[0] = Status,           /* Status */
        .arg[1] = CallbackArg,      /* CallbackArg */
        .arg[2] = BARNum,           /* BARNum */
        .arg[3] = NULL,             /* GP1 */
        .arg[4] = NULL,             /* GP2 */
        .arg[5] = NULL,             /* GP3 */
    };

    Sys_Execthread(Callback, &arguments, ExecutionTypeQueu, NULL);
    Sys_Close(KSUCCESS);
}

KResult GetBARType(thread_t Callback, uint64_t CallbackArg, PCIDeviceID_t Device, uint8_t BarIndex){
    KResult Status = KFAIL;
    uint8_t BARType = NULL;
    if(CheckDevice(Device)){
        if(BarIndex < GetDevice(Device)->BARNum){
            BARType = GetDevice(Device)->BAR[BarIndex]->Type;
            Status = KSUCCESS;
        }
    }

    arguments_t arguments{
        .arg[0] = Status,           /* Status */
        .arg[1] = CallbackArg,      /* CallbackArg */
        .arg[2] = BARType,          /* BARType */
        .arg[3] = NULL,             /* GP1 */
        .arg[4] = NULL,             /* GP2 */
        .arg[5] = NULL,             /* GP3 */
    };

    Sys_Execthread(Callback, &arguments, ExecutionTypeQueu, NULL);
    Sys_Close(KSUCCESS);
}

KResult GetBARSize(thread_t Callback, uint64_t CallbackArg, PCIDeviceID_t Device, uint8_t BarIndex){
    KResult Status = KFAIL;
    uint64_t BARSize = NULL;
    if(CheckDevice(Device)){
        if(BarIndex < GetDevice(Device)->BARNum){
            BARSize = GetDevice(Device)->BAR[BarIndex]->Size;
            Status = KSUCCESS;
        }
    }

    arguments_t arguments{
        .arg[0] = Status,           /* Status */
        .arg[1] = CallbackArg,      /* CallbackArg */
        .arg[2] = BARSize,          /* BARSize */
        .arg[3] = NULL,             /* GP1 */
        .arg[4] = NULL,             /* GP2 */
        .arg[5] = NULL,             /* GP3 */
    };

    Sys_Execthread(Callback, &arguments, ExecutionTypeQueu, NULL);
    Sys_Close(KSUCCESS);
}

KResult PCISearcher(thread_t Callback, uint64_t CallbackArg, srv_pci_search_parameters_t* SearchParameters){
    KResult Status = KFAIL;
    uint64_t NumDeviceFound = Search(SearchParameters->vendorID, SearchParameters->deviceID, SearchParameters->subClassID, SearchParameters->classID, SearchParameters->progIF);
    
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

KResult PCISearcherGetDevice(thread_t Callback, uint64_t CallbackArg, srv_pci_search_parameters_t* SearchParameters, uint64_t Index){
    KResult Status = KFAIL;
    PCIDeviceID_t Device = NULL;
    if(CheckDevice(Device)){
        Device = Search(SearchParameters->vendorID, SearchParameters->deviceID, SearchParameters->subClassID, SearchParameters->progIF, Index);
    }

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