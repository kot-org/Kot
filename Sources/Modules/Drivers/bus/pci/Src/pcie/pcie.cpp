#include <pcie/pcie.h>

void InitPCIe(PCIDeviceListInfo_t* PCIDeviceList, void* mcfgAddress){
    MCFGHeader_t* MCFG = (MCFGHeader_t*)mcfgAddress;

    size64_t Entries = ((MCFG->Header.Length) - sizeof(MCFGHeader_t)) / sizeof(DeviceConfig_t);

    for(size64_t i = 0; i < Entries; i++){
        DeviceConfig_t* DeviceConfig = &MCFG->ConfigurationSpace[i];
        for (uint64_t bus = DeviceConfig->StartBus; bus < DeviceConfig->EndBus; bus++){
            EnumerateBus(PCIDeviceList, (void*)DeviceConfig->BaseAddress, bus);
        }
    }
}


void EnumerateBus(PCIDeviceListInfo_t* PCIDeviceList, void* baseAddress, uint64_t bus){
    uint64_t offset = bus << 20;

    void* busAddress = (void*)((uint64_t)baseAddress + offset);
    PCIDeviceHeader_t* PCIDeviceHeader = (PCIDeviceHeader_t*)MapPhysical(busAddress, PCIE_CONFIGURATION_SPACE_SIZE);
    if(PCIDeviceHeader->DeviceID == 0) return;
    if(PCIDeviceHeader->DeviceID == 0xFFFF) return;

    for(uint64_t device = 0; device < 32; device++){
        EnumerateDevice(PCIDeviceList, busAddress, device);
    }
}

void EnumerateDevice(PCIDeviceListInfo_t* PCIDeviceList, void* busAddress, uint64_t device){
    uint64_t offset = device << 15;

    void* deviceAddress = (void*)((uint64_t)busAddress + offset);
    PCIDeviceHeader_t* PCIDeviceHeader = (PCIDeviceHeader_t*)MapPhysical(deviceAddress, PCIE_CONFIGURATION_SPACE_SIZE);

    if(PCIDeviceHeader->DeviceID == 0) return;
    if(PCIDeviceHeader->DeviceID == 0xFFFF) return;

    if((PCIDeviceHeader->HeaderType & 0x80) != 0){
        for(uint64_t function = 0; function < 8; function++){
            EnumerateFunction(PCIDeviceList, deviceAddress, function);
        }
    }else{
        EnumerateFunction(PCIDeviceList, deviceAddress, NULL);
    }
}

void EnumerateFunction(PCIDeviceListInfo_t* PCIDeviceList, void* deviceAddress, uint64_t function){
    uint64_t offset = function << 12;

    void* functionAddress = (void*)((uint64_t)deviceAddress + offset);
    PCIDeviceHeader_t* PCIDeviceHeader = (PCIDeviceHeader_t*)MapPhysical(functionAddress, PCIE_CONFIGURATION_SPACE_SIZE);

    if(PCIDeviceHeader->DeviceID == 0) return;
    if(PCIDeviceHeader->DeviceID == 0xFFFF) return;

    PCIDevice_t* Device = (PCIDevice_t*)malloc(sizeof(PCIDevice_t));
    Device->IsPCIe = true;
    Device->Address = NULL; // this is not pci device, it's pcie device
    Device->ConfigurationSpace = PCIDeviceHeader;

    AddPCIDevice(PCIDeviceList, Device);

    return;
}

void ReceiveConfigurationSpacePCIe(PCIDevice_t* Device){
    // Don't do anything on pci express because it's alread memory mapped
}

void SendConfigurationSpacePCIe(PCIDevice_t* Device){
    // Don't do anything on pci express because it's alread memory mapped
}