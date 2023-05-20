#ifndef KOT_SRV_PCI_H
#define KOT_SRV_PCI_H 1

#include <stdlib.h>
#include <string.h>

#include <kot/sys.h>
#include <kot/uisd.h>
#include <kot/types.h>
#include <kot/atomic.h>
#include <kot/memory.h>

#define PCI_BAR_TYPE_NULL           0x0
#define PCI_BAR_TYPE_IO             0x1
#define PCI_BAR_TYPE_32             0x2
#define PCI_BAR_TYPE_64             0x3

#define PCI_SEARCH_NO_PARAMETER     0xFFFF

#define PCI_MSI_VERSION             0x1
#define PCI_MSIX_VERSION            0x2

typedef KResult (*kot_PCICallbackHandler)(KResult Status, struct kot_srv_pci_callback_t* Callback, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3);

typedef uint64_t kot_PCIDeviceID_t;

typedef struct {
    uint16_t vendorID;
    uint16_t deviceID;
    uint16_t classID;
    uint16_t subClassID;
    uint16_t progIF;
} kot_srv_pci_search_parameters_t;

typedef struct {
    uint16_t vendorID;
    uint16_t deviceID;
    uint16_t classID;
    uint16_t subClassID;
    uint16_t progIF;
} kot_srv_pci_device_info_t;

typedef struct {
    uintptr_t Address;
    size64_t Size;
    uint8_t Type;
} kot_srv_pci_bar_info_t;

struct kot_srv_pci_callback_t{
    kot_thread_t Self;
    uint64_t Data;
    size64_t Size;
    bool IsAwait;
    KResult Status;
    kot_PCICallbackHandler Handler;
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

void kot_Srv_Pci_Initialize();

void kot_Srv_Pci_Callback(KResult Status, struct kot_srv_pci_callback_t* Callback, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3);

struct kot_srv_pci_callback_t* kot_Srv_Pci_CountDevices(kot_srv_pci_search_parameters_t* SearchParameters, bool IsAwait);
struct kot_srv_pci_callback_t* kot_Srv_Pci_FindDevice(kot_srv_pci_search_parameters_t* SearchParameters, uint64_t Index, bool IsAwait);
struct kot_srv_pci_callback_t* kot_Srv_Pci_GetInfoDevice(kot_PCIDeviceID_t Device, bool IsAwait);
struct kot_srv_pci_callback_t* kot_Srv_Pci_GetBAR(kot_PCIDeviceID_t Device, uint8_t BarIndex, bool IsAwait);
struct kot_srv_pci_callback_t* kot_Srv_Pci_BindMSI(kot_PCIDeviceID_t Device, uint8_t IRQVector, uint8_t Processor, uint16_t LocalDeviceVector, bool IsAwait);
struct kot_srv_pci_callback_t* kot_Srv_Pci_UnbindMSI(kot_PCIDeviceID_t Device, uint16_t LocalDeviceVector, bool IsAwait);
struct kot_srv_pci_callback_t* kot_Srv_Pci_ConfigReadWord(kot_PCIDeviceID_t Device, uint16_t Offset, bool IsAwait);
struct kot_srv_pci_callback_t* kot_Srv_Pci_ConfigWriteWord(kot_PCIDeviceID_t Device, uint16_t Offset, uint16_t Value, bool IsAwait);

inline uint16_t kot_PCIGetCommand(kot_PCIDeviceID_t Device){ 
    struct kot_srv_pci_callback_t* Callback = kot_Srv_Pci_ConfigReadWord(Device, PCI_REGISTERS_Command, true);
    uint16_t Value = Callback->Data;
    free(Callback);
    return Value;
}

inline uint16_t kot_PCISetCommand(kot_PCIDeviceID_t Device, uint16_t Value){ 
    struct kot_srv_pci_callback_t* Callback = kot_Srv_Pci_ConfigWriteWord(Device, PCI_REGISTERS_Command, Value, true);
    free(Callback);
    return Value;
}

inline void kot_PCIEnableBusMastering(kot_PCIDeviceID_t Device){ 
    kot_PCISetCommand(Device, kot_PCIGetCommand(Device) | PCI_COMMAND_BusMastering); 
}

inline void kot_PCIEnableInterrupts(kot_PCIDeviceID_t Device){ 
    kot_PCISetCommand(Device, kot_PCIGetCommand(Device) & (~PCI_COMMAND_InterruptDisable)); 
}

inline void kot_PCIEnableMemorySpace(kot_PCIDeviceID_t Device){ 
    kot_PCISetCommand(Device, kot_PCIGetCommand(Device) | PCI_COMMAND_MemorySpace); 
}

inline void kot_PCIEnableIOSpace(kot_PCIDeviceID_t Device){ 
    kot_PCISetCommand(Device, kot_PCIGetCommand(Device) | PCI_COMMAND_IoSpace); 
}

#endif