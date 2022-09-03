#include <core/main.h>

process_t Proc = NULL;

AHCIController** Controllers = NULL;

Device** Devices = NULL;
uint64_t DevicesIndex = 0;

extern "C" int main(int argc, char* argv[]) {
    Printlog("[AHCI] Initialization ...");

    Proc = Sys_GetProcess();

    srv_pci_search_parameters_t SearchParameters{
        .vendorID = PCI_SEARCH_NO_PARAMETER,
        .deviceID = PCI_SEARCH_NO_PARAMETER,
        .classID = 0x1,
        .subClassID = 0x6,
        .progIF = 0x1,
    };

    srv_pci_callback_t* Callback = Srv_Pci_CountDevices(&SearchParameters, true);
    uint64_t DevicesNumber = (uint64_t)Callback->Data;
    free(Callback);
    
    Controllers = (AHCIController**)malloc(DevicesNumber * sizeof(AHCIController*));

    Devices = (Device**)malloc(DevicesNumber * PORT_MAX_COUNT * sizeof(Device*));
    DevicesIndex = 0;

    for(uint64_t i = 0; i < DevicesNumber; i++){
        Callback = Srv_Pci_FindDevice(&SearchParameters, i, true);
        PCIDeviceID_t DeviceID = (PCIDeviceID_t)Callback->Data;
        free(Callback);

        Callback = Srv_Pci_GetBAR(DeviceID, 0x5, true);
        srv_pci_bar_info_t* BarInfo = (srv_pci_bar_info_t*)Callback->Data;
        free(Callback);
        
        Controllers[i] = new AHCIController(BarInfo);
    }

    Printlog("[AHCI] Driver initialized successfully");

    return KSUCCESS;
}

void AddDevice(Device* Device){
    Devices[DevicesIndex] = Device;
    DevicesIndex++;
}

Device* GetDevice(uint64_t Index){
    if(Index >= DevicesIndex) return NULL;
    return Devices[Index];
}

KResult Read(uint64_t Index, uint64_t Start, size64_t Size){
    Device* self = GetDevice(Index);
    if(self){
        return self->Read(Start, Size);
    }
    return KFAIL;
}

KResult Write(uint64_t Index, uint64_t Start, size64_t Size){
    Device* self = GetDevice(Index);
    if(self){
        return self->Write(Start, Size);
    }
    return KFAIL;
}