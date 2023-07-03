#include <core/main.h>

kot_process_t Proc = NULL;

int main(int argc, char* argv[]) {
    kot_Printlog("[NET/E1000] Initialization ...");

    kot_srv_pci_search_parameters_t SearchParameters{
        .vendorID = INTEL_VEND,
        .deviceID = E1000_DEV,
        .classID = PCI_SEARCH_NO_PARAMETER,
        .subClassID = PCI_SEARCH_NO_PARAMETER,
        .progIF = PCI_SEARCH_NO_PARAMETER,
    };

    Proc = kot_Sys_GetProcess();

    kot_srv_pci_callback_t* Callback = kot_Srv_Pci_FindDevice(&SearchParameters, 0, true);
    kot_PCIDeviceID_t DeviceID = (kot_PCIDeviceID_t)Callback->Data;
    free(Callback);

    if(DeviceID == NULL) {
        kot_Printlog("[NET/E1000] Intel network cards not found (exit)");
        return KFAIL;
    }

    Callback = kot_Srv_Pci_GetInfoDevice(DeviceID, true);
    kot_srv_pci_device_info_t* DeviceInfo = (kot_srv_pci_device_info_t*)Callback->Data;
    free(Callback);

    Callback = kot_Srv_Pci_GetBAR(DeviceID, 0x0, true);
    kot_srv_pci_bar_info_t* BarInfo = (kot_srv_pci_bar_info_t*)Callback->Data;
    free(Callback);

    E1000* controller = new E1000(DeviceInfo, BarInfo);

    kot_Printlog("[NET/E1000] Driver initialized successfully");

    kot_Sys_Close(KSUCCESS); /* Don't close the process */
}