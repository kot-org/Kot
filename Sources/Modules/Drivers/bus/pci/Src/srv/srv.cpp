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

    /* PCISearcherGetDevice */
    thread_t PCISearcherGetDeviceThread = NULL;
    Sys_Createthread(proc, (uintptr_t) &PCISearcherGetDevice, PriviledgeDriver, &PCISearcherGetDeviceThread);
    PciSrv->PCISearcherGetDevice = MakeShareableThread(PCISearcherGetDeviceThread, PriviledgeDriver);
    
    /* PCISearcher */
    thread_t PCISearcherThread = NULL;
    Sys_Createthread(proc, (uintptr_t) &PCISearcher, PriviledgeDriver, &PCISearcherThread);
    PciSrv->PCISearcher = MakeShareableThread(PCISearcherThread, PriviledgeDriver);
}

bool checkDeviceIndex(uint32_t index) {
    if(index < PCIDevicesIndex)
        return true;
    return false;
}

KResult GetBARNum(uint32_t index) {
    if(checkDeviceIndex(index))
        Sys_Close(PCIDevices[index]->BARNum);
    Sys_Close(KSUCCESS);
}
KResult GetBARType(uint32_t index, uint8_t barIndex) {
    if(checkDeviceIndex(index) && barIndex < PCIDevices[index]->BARNum)
        Sys_Close(PCIDevices[index]->BAR[barIndex]->Type);
    Sys_Close(KSUCCESS);
}
KResult GetBARSize(uint32_t index, uint8_t barIndex) {
    if(checkDeviceIndex(index) && barIndex < PCIDevices[index]->BARNum)
        Sys_Close(PCIDevices[index]->BAR[barIndex]->Size);
    Sys_Close(KSUCCESS);
}

KResult PCISearcherGetDevice(uint16_t vendorID, uint16_t deviceID, uint16_t subClassID, uint16_t classID, uint16_t progIf, uint64_t index) {
    uint8_t checkRequired = 0;
    uint32_t deviceNum = 0;

    if(vendorID != 0xFFFF)
        checkRequired++;
    if(deviceID != 0xFFFF)
        checkRequired++;
    if(subClassID != 0xFFFF)
        checkRequired++;
    if(classID != 0xFFFF)
        checkRequired++;
    if(progIf != 0xFFFF)
        checkRequired++;

    for(uint32_t i = 0; i < PCIDevicesIndex; i++) {
        
        PCIDeviceHeader header = ((PCIHeader0*)PCIDevices[i])->Header;

        uint8_t checkNum = 0;
        
        if(header.VendorID == vendorID)
            checkNum++;
        if(header.DeviceID == deviceID)
            checkNum++;
        if(header.Subclass == subClassID)
            checkNum++;
        if(header.Class == classID)
            checkNum++;
        if(header.ProgIF == progIf)
            checkNum++;

        if(checkRequired == checkNum) deviceNum++;

        if(index == deviceNum)
            Sys_Close(i);

    }
    Sys_Close(KSUCCESS);
}

KResult PCISearcher(uint16_t vendorID, uint16_t deviceID, uint16_t subClassID, uint16_t classID, uint16_t progIf) {
    uint8_t checkRequired = 0;
    uint32_t deviceNum = 0;

    if(vendorID != 0xFFFF)
        checkRequired++;
    if(deviceID != 0xFFFF)
        checkRequired++;
    if(subClassID != 0xFFFF)
        checkRequired++;
    if(classID != 0xFFFF)
        checkRequired++;
    if(progIf != 0xFFFF)
        checkRequired++;

    for(uint32_t i = 0; i < PCIDevicesIndex; i++) {

        PCIDeviceHeader header = ((PCIHeader0*)PCIDevices[i])->Header;

        uint8_t checkNum = 0;
        
        if(header.VendorID == vendorID)
            checkNum++;
        if(header.DeviceID == deviceID)
            checkNum++;
        if(header.Subclass == subClassID)
            checkNum++;
        if(header.Class == classID)
            checkNum++;
        if(header.ProgIF == progIf)
            checkNum++;

        if(checkRequired == checkNum) deviceNum++;

    }
    Sys_Close(deviceNum);
}