#include <core/main.h>

process_t Proc = NULL;

extern "C" int main(int argc, char* argv[]) {
    kot_Printlog("[NET/E1000] Initialization ...");

    srv_pci_search_parameters_t SearchParameters{
        .vendorID = INTEL_VEND,
        .deviceID = E1000_DEV,
        .classID = PCI_SEARCH_NO_PARAMETER,
        .subClassID = PCI_SEARCH_NO_PARAMETER,
        .progIF = PCI_SEARCH_NO_PARAMETER,
    };

    Proc = Sys_GetProcess();

    srv_pci_callback_t* Callback = Srv_Pci_FindDevice(&SearchParameters, 0, true);
    kot_PCIDeviceID_t DeviceID = (kot_PCIDeviceID_t)Callback->Data;
    free(Callback);

    if(DeviceID == NULL) {
        kot_Printlog("[NET/E1000] Intel network cards not found (exit)");
        return KFAIL;
    }

    Callback = Srv_Pci_GetInfoDevice(DeviceID, true);
    srv_pci_device_info_t* DeviceInfo = (srv_pci_device_info_t*)Callback->Data;
    free(Callback);

    Callback = Srv_Pci_GetBAR(DeviceID, 0x0, true);
    srv_pci_bar_info_t* BarInfo = (srv_pci_bar_info_t*)Callback->Data;
    free(Callback);

    E1000* controller = new E1000(DeviceInfo, BarInfo);

    kot_Printlog("[NET/E1000] Driver initialized successfully");

    return KSUCCESS;
}