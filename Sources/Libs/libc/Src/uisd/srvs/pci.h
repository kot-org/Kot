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

#define PCI_MSI_VERSION             0x1
#define PCI_MSIX_VERSION            0x2

#if defined(__cplusplus)
extern "C" {
#endif

typedef KResult (*PCICallbackHandler)(KResult Status, struct srv_pci_callback_t* Callback, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3);

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
    void* Address;
    size64_t Size;
    uint8_t Type;
} srv_pci_bar_info_t;

struct srv_pci_callback_t{
    thread_t Self;
    uint64_t Data;
    size64_t Size;
    bool IsAwait;
    KResult Status;
    PCICallbackHandler Handler;
};

#define PCI_REGISTERS_VendorID                  0x0
#define PCI_REGISTERS_DeviceID                  0x2
#define PCI_REGISTERS_Command                   0x4
#define PCI_REGISTERS_Status                    0x6
#define PCI_REGISTERS_RevisionID                0x8
#define PCI_REGISTERS_Subclass                  0xA
#define PCI_REGISTERS_ClassCode                 0xB
#define PCI_REGISTERS_CacheLineSize             0xC
#define PCI_REGISTERS_ProgIF                    0x9
#define PCI_REGISTERS_LatencyTimer              0xD
#define PCI_REGISTERS_HeaderType                0xE
#define PCI_REGISTERS_BIST                      0xF
#define PCI_REGISTERS_BAR0                      0x10
#define PCI_REGISTERS_BAR1                      0x14
#define PCI_REGISTERS_BAR2                      0x18
#define PCI_REGISTERS_BAR3                      0x1C
#define PCI_REGISTERS_BAR4                      0x20
#define PCI_REGISTERS_BAR5                      0x24
#define PCI_REGISTERS_CardbusCISPointer         0x28
#define PCI_REGISTERS_SubsystemID               0x2E
#define PCI_REGISTERS_SubsystemVendorID         0x2C
#define PCI_REGISTERS_ExpansionROMBaseAddress   0x30
#define PCI_REGISTERS_CapabilitiesPointer       0x34
#define PCI_REGISTERS_MaxLatency                0x3F
#define PCI_REGISTERS_MinGrant                  0x3E
#define PCI_REGISTERS_InterruptPIN              0x3D
#define PCI_REGISTERS_InterruptLine             0x3C

#define PCI_COMMAND_IoSpace                     (1 << 0x0)
#define PCI_COMMAND_MemorySpace                 (1 << 0x1)
#define PCI_COMMAND_BusMastering                (1 << 0x2)
#define PCI_COMMAND_InterruptDisable            (1 << 0xA)

void Srv_Pci_Initialize();

void Srv_Pci_Callback(KResult Status, struct srv_pci_callback_t* Callback, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3);

struct srv_pci_callback_t* Srv_Pci_CountDevices(srv_pci_search_parameters_t* SearchParameters, bool IsAwait);
struct srv_pci_callback_t* Srv_Pci_FindDevice(srv_pci_search_parameters_t* SearchParameters, uint64_t Index, bool IsAwait);
struct srv_pci_callback_t* Srv_Pci_GetInfoDevice(PCIDeviceID_t Device, bool IsAwait);
struct srv_pci_callback_t* Srv_Pci_GetBAR(PCIDeviceID_t Device, uint8_t BarIndex, bool IsAwait);
struct srv_pci_callback_t* Srv_Pci_BindMSI(PCIDeviceID_t Device, uint8_t IRQVector, uint8_t Processor, uint16_t LocalDeviceVector, bool IsAwait);
struct srv_pci_callback_t* Srv_Pci_UnbindMSI(PCIDeviceID_t Device, uint16_t LocalDeviceVector, bool IsAwait);
struct srv_pci_callback_t* Srv_Pci_ConfigReadWord(PCIDeviceID_t Device, uint16_t Offset, bool IsAwait);
struct srv_pci_callback_t* Srv_Pci_ConfigWriteWord(PCIDeviceID_t Device, uint16_t Offset, uint16_t Value, bool IsAwait);

inline uint16_t PCIGetCommand(PCIDeviceID_t Device){ 
    struct srv_pci_callback_t* Callback = Srv_Pci_ConfigReadWord(Device, PCI_REGISTERS_Command, true);
    uint16_t Value = Callback->Data;
    free(Callback);
    return Value;
}

inline uint16_t PCISetCommand(PCIDeviceID_t Device, uint16_t Value){ 
    struct srv_pci_callback_t* Callback = Srv_Pci_ConfigWriteWord(Device, PCI_REGISTERS_Command, Value, true);
    free(Callback);
    return Value;
}

inline void PCIEnableBusMastering(PCIDeviceID_t Device){ 
    PCISetCommand(Device, PCIGetCommand(Device) | PCI_COMMAND_BusMastering); 
}

inline void PCIEnableInterrupts(PCIDeviceID_t Device){ 
    PCISetCommand(Device, PCIGetCommand(Device) & (~PCI_COMMAND_InterruptDisable)); 
}

inline void PCIEnableMemorySpace(PCIDeviceID_t Device){ 
    PCISetCommand(Device, PCIGetCommand(Device) | PCI_COMMAND_MemorySpace); 
}

inline void PCIEnableIOSpace(PCIDeviceID_t Device){ 
    PCISetCommand(Device, PCIGetCommand(Device) | PCI_COMMAND_IoSpace); 
}

#if defined(__cplusplus)
}
#endif

#endif