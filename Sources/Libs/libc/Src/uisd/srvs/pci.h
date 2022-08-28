#ifndef _SRV_PCI_H
#define _SRV_PCI_H 1

#include <kot/sys.h>
#include <kot/uisd.h>

#if defined(__cplusplus)
extern "C" {
#endif

typedef KResult (*CallbackHandler)(KResult Status, struct srv_pci_callback* Callback, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3);

typedef struct {
    uint16_t VendorID;
    uint16_t DeviceID;
    uint16_t Command;
    uint16_t Status;
    uint8_t RevisionID;
    uint8_t ProgIF;
    uint8_t Subclass;
    uint8_t Class;
    uint8_t CacheLineSize;
    uint8_t LatencyTimer;
    uint8_t HeaderType;
    uint8_t BIST;
} srv_pci_device_header_t;

typedef struct {
    srv_pci_device_header_t Header;
    uint32_t BAR[6];
    uint32_t CardBusCISPtr;
    uint16_t SubsystemVendorID;
    uint16_t SubsystemID;
    uint32_t ExpansionRomBaseAddr;
    uint8_t CapabilitiesPtr;
    uint8_t  Reserved0;
    uint16_t Reserved1;
    uint32_t Reserved2;
    uint8_t IntreruptLine;
    uint8_t IntreruptPin;
    uint8_t MinGrant;
    uint8_t MaxLatency;
} srv_pci_header0_t;

typedef struct {
    uint64_t Base;
    uint64_t Size;
    uint8_t Type;
} srv_pci_bar_t;

typedef struct {
    srv_pci_device_header_t* Header;
    srv_pci_bar_t* BAR[6];
    uint8_t BARNum;
    uint32_t Index;
} srv_pci_device_t;

typedef struct srv_pci_callback {
    thread_t Self;
    uintptr_t Data;
    size64_t Size;
    bool IsAwait;
    KResult Status;
    CallbackHandler Handler;
} srv_pci_callback_t;

void SrvPciInitialize();

void SrvPciCallback(KResult Status, srv_pci_callback_t* Callback, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3);

#if defined(__cplusplus)
}
#endif

#endif