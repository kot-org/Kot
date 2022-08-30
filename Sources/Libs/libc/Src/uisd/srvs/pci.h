#ifndef _SRV_PCI_H
#define _SRV_PCI_H 1

#include <kot/sys.h>
#include <kot/uisd.h>
#include <kot/types.h>

#define PCI_BAR_TYPE_NULL           0x0
#define PCI_BAR_TYPE_IO             0x1
#define PCI_BAR_TYPE_32             0x2
#define PCI_BAR_TYPE_64             0x3

#define PCI_SEARCH_NO_PARAMETER     0xFFFF

#if defined(__cplusplus)
extern "C" {
#endif

typedef KResult (*PCICallbackHandler)(KResult Status, struct srv_pci_callback* Callback, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3);

typedef uint64_t PCIDeviceID_t;

typedef struct {
    uint16_t vendorID;
    uint16_t deviceID;
    uint16_t classID;
    uint16_t subClassID;
    uint16_t progIF;
} srv_pci_search_parameters_t;

typedef struct {
    uint16_t vendorID;
    uint16_t deviceID;
    uint16_t classID;
    uint16_t subClassID;
    uint16_t progIF;
} srv_pci_device_info_t;

typedef struct {
    uintptr_t Address;
    size64_t Size;
    uint8_t Type;
} srv_pci_bar_info_t;

struct srv_pci_callback_t{
    thread_t Self;
    uintptr_t Data;
    size64_t Size;
    bool IsAwait;
    KResult Status;
    PCICallbackHandler Handler;
};

void Srv_Pci_Initialize();

void Srv_Pci_Callback(KResult Status, struct srv_pci_callback_t* Callback, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3);

struct srv_pci_callback_t* Srv_Pci_CountDevices(srv_pci_search_parameters_t* SearchParameters, bool IsAwait);
struct srv_pci_callback_t* Srv_Pci_FindDevice(srv_pci_search_parameters_t* SearchParameters, uint64_t Index, bool IsAwait);
struct srv_pci_callback_t* Srv_Pci_GetInfoDevice(PCIDeviceID_t Device, bool IsAwait);
struct srv_pci_callback_t* Srv_Pci_GetBAR(PCIDeviceID_t Device, uint8_t BarIndex, bool IsAwait);
struct srv_pci_callback_t* Srv_Pci_SetupMSI(PCIDeviceID_t Device, uint8_t IRQVector, uint16_t LocalDeviceVector, bool IsAwait);


#if defined(__cplusplus)
}
#endif

#endif