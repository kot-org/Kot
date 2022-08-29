#include <core/main.h>

extern "C" int main(int argc, char* argv[]) {
    Printlog("[AHCI] Initialization ...");

    srv_pci_search_parameters_t SearchParameters{
        .vendorID = PCI_SEARCH_NO_PARAMETER,
        .deviceID = PCI_SEARCH_NO_PARAMETER,
        .classID = 0x1,
        .subClassID = 0x6,
        .progIF = 0x1,
    };

    srv_pci_callback_t* Callback = Srv_Pci_CountDevices(&SearchParameters, true);
    uint64_t DeivicesNumber = (uint64_t)Callback->Data;
    free(Callback);

    for(uint64_t i = 0; i < DeivicesNumber; i++){
        Callback = Srv_Pci_FindDevice(&SearchParameters, i, true);
        PCIDeviceID_t DeviceID = (PCIDeviceID_t)Callback->Data;
        free(Callback);

        Callback = Srv_Pci_GetBAR(DeviceID, 0x5, true);
        srv_pci_bar_info_t* BarInfo = (srv_pci_bar_info_t*)Callback->Data;
        free(Callback);
        
        std::printf("[AHCI] %x", BarInfo->Size);
    }

    Printlog("[AHCI] Driver initialized successfully");

    return KSUCCESS;
}