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

    std::printf("[AHCI] %x", Callback->Data);

    Printlog("[AHCI] Driver initialized successfully");

    return KSUCCESS;
}