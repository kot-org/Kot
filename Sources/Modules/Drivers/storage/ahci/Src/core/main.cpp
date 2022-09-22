#include <core/main.h>

process_t Proc = NULL;

AHCIController** Controllers = NULL;

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