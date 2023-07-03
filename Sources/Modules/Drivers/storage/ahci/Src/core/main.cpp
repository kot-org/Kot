#include <core/main.h>

kot_process_t Proc = NULL;

AHCIController** Controllers = NULL;

int main(int argc, char* argv[]) {
    kot_Printlog("[STORAGE/AHCI] Initialization ...");

    Proc = kot_Sys_GetProcess();

    kot_srv_pci_search_parameters_t SearchParameters{
        .vendorID = PCI_SEARCH_NO_PARAMETER,
        .deviceID = PCI_SEARCH_NO_PARAMETER,
        .classID = 0x1,
        .subClassID = 0x6,
        .progIF = 0x1,
    };

    kot_srv_pci_callback_t* Callback = kot_Srv_Pci_CountDevices(&SearchParameters, true);
    uint64_t DevicesNumber = (uint64_t)Callback->Data;
    free(Callback);
    
    Controllers = (AHCIController**)malloc(DevicesNumber * sizeof(AHCIController*));

    for(uint64_t i = 0; i < DevicesNumber; i++){
        Callback = kot_Srv_Pci_FindDevice(&SearchParameters, i, true);
        kot_PCIDeviceID_t DeviceID = (kot_PCIDeviceID_t)Callback->Data;
        free(Callback);

        Callback = kot_Srv_Pci_GetBAR(DeviceID, 0x5, true);
        kot_srv_pci_bar_info_t* BarInfo = (kot_srv_pci_bar_info_t*)Callback->Data;
        free(Callback);
        
        Controllers[i] = new AHCIController(BarInfo);
    }

    kot_Printlog("[STORAGE/AHCI] Driver initialized successfully");

    kot_Sys_Close(KSUCCESS); /* Don't close the process */
}