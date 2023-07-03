#include <core/main.h>

kot_process_t Proc = NULL;

int main(int argc, char* argv[]) {
    kot_Printlog("[AUDIO/HDA] Initialization ...");

    kot_srv_pci_search_parameters_t SearchParameters{
        .vendorID = PCI_SEARCH_NO_PARAMETER,
        .deviceID = PCI_SEARCH_NO_PARAMETER,
        .classID = HDA_CLASS,
        .subClassID = HDA_SUBCLASS,
        .progIF = PCI_SEARCH_NO_PARAMETER,
    };

    Proc = kot_Sys_GetProcess();

    kot_srv_pci_callback_t* Callback = kot_Srv_Pci_CountDevices(&SearchParameters, true);
    uint64_t DevicesNumber = (uint64_t)Callback->Data;
    free(Callback);

    for(uint64_t i = 0; i < DevicesNumber; i++){
        Callback = kot_Srv_Pci_FindDevice(&SearchParameters, i, true);
        kot_PCIDeviceID_t DeviceID = (kot_PCIDeviceID_t)Callback->Data;
        free(Callback);
        new HDAController(DeviceID);
    }

    kot_Printlog("[AUDIO/HDA] Driver initialized successfully");

    kot_Sys_Close(KSUCCESS); /* Don't close the process */
}