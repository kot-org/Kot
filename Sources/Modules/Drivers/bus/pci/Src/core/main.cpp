#include <core/main.h>

#include <tools/config.h>
#include <tools/memory.h>

process_t proc;

PCIDevice_t** PCIDevices = NULL;
PCIDeviceID_t PCIDevicesIndex = 0;

bool CheckDevice(PCIDeviceID_t device){
    if(PCIDevicesIndex != NULL && device < PCIDevicesIndex){
        return true;
    }
    return false;
}

PCIDevice_t* GetDevice(PCIDeviceID_t device){
    return PCIDevices[device];
}


extern "C" int main(int argc, char* argv[]) {
    Printlog("[PCI] Initialization ...");
    
    EnumerateDevices();

    InitSrv();

    Printlog("[PCI] Driver initialized successfully");

    return KSUCCESS;
}