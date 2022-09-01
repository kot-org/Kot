#include <controller/controller.h>

#include <kot++/printf.h>

using namespace std;

E1000Controller** Controllers = NULL;

extern "C" int main(int argc, char* argv[]) {
    Printlog("[NET/E1000] Initialization ...");

    srv_pci_search_parameters_t SearchParameters{
        .vendorID = INTEL_VEND,
        .deviceID = E1000_DEV,
        .classID = PCI_SEARCH_NO_PARAMETER,
        .subClassID = PCI_SEARCH_NO_PARAMETER,
        .progIF = PCI_SEARCH_NO_PARAMETER,
    };

    srv_pci_callback_t* Callback = Srv_Pci_CountDevices(&SearchParameters, true);
    uint64_t DevicesNumber = (uint64_t)Callback->Data;
    free(Callback);

    Controllers = (E1000Controller**) malloc(DevicesNumber * sizeof(E1000Controller*));

    for(uint32_t i = 0; i < DevicesNumber; i++) {
        Callback = Srv_Pci_FindDevice(&SearchParameters, i, true);
        PCIDeviceID_t DeviceID = (PCIDeviceID_t)Callback->Data;
        free(Callback);

        Callback = Srv_Pci_GetInfoDevice(DeviceID, true);
        srv_pci_device_info_t* DeviceInfo = (srv_pci_device_info_t*)Callback->Data;
        free(Callback);

        if(DeviceInfo != NULL) {
            Callback = Srv_Pci_GetBAR(DeviceID, 0x0, true);
            srv_pci_bar_info_t* BarInfo = (srv_pci_bar_info_t*)Callback->Data;
            free(Callback);

            Controllers[i] = new E1000Controller(BarInfo);
        } else {
            Printlog("[NET/E1000] Intel network cards not found");
        }
    }

    Printlog("[NET/E1000] Driver initialized successfully");

    return KSUCCESS;
}