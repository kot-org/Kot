#include <core/main.h>

#include <tools/config.h>
#include <tools/memory.h>

process_t proc;

PCIDevice_t** PCIDevices = NULL;
PCIDeviceID_t PCIDevicesIndex;

extern "C" int main(int argc, char* argv[]) {
    Printlog("[PCI] Initialization ...");
    
    PCIDevicesIndex = 1;

    EnumerateDevices();

    InitSrv();

    Printlog("[PCI] Driver initialized successfully");

    return KSUCCESS;
}

bool CheckDevice(PCIDeviceID_t device){
    if(PCIDevicesIndex != NULL && device < PCIDevicesIndex){
        return true;
    }
    return false;
}

PCIDevice_t* GetDevice(PCIDeviceID_t device){
    return PCIDevices[device];
}

uint64_t Search(uint16_t vendorID, uint16_t deviceID, uint16_t subClassID, uint16_t classID, uint16_t progIF){
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
    if(progIF != 0xFFFF)
        checkRequired++;

    for(uint32_t i = 1; i < PCIDevicesIndex; i++) {

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
        if(header.ProgIF == progIF)
            checkNum++;

        if(checkRequired == checkNum) deviceNum++;

    }
    return deviceNum;
}

PCIDeviceID_t GetDevice(uint16_t vendorID, uint16_t deviceID, uint16_t subClassID, uint16_t classID, uint16_t progIF, uint64_t index){
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
    if(progIF != 0xFFFF)
        checkRequired++;

    for(uint32_t i = 1; i < PCIDevicesIndex; i++) {
        
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
        if(header.ProgIF == progIF)
            checkNum++;

        if(checkRequired == checkNum) deviceNum++;

        if(index == deviceNum){
            return i;
        }

    }
    return NULL;
}