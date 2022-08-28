#ifndef _SRV_PCI_H
#define _SRV_PCI_H 1

#include <kot/sys.h>
#include <kot/uisd.h>
#include <kot/types.h>

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

#if defined(__cplusplus)
}
#endif

#endif