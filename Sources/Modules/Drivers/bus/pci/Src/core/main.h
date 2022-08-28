#pragma once

#define Pci_Srv_Version 0x1

#define PCI_VENDOR_ID_OFFSET 		0x0
#define PCI_DEVICE_ID_OFFSET 		0x2
#define PCI_COMMAND_OFFSET 			0x4
#define PCI_STATUS_OFFSET 			0x6
#define PCI_REVISION_ID_OFFSET 		0x8
#define PCI_PROG_IF_OFFSET 			0x9
#define PCI_SUBCLASS_OFFSET 		0xA
#define PCI_CLASS_CODE_OFFSET 		0xB
#define PCI_CACHE_SIZE_OFFSET 		0xC
#define PCI_LATENCY_TIMER_OFFSET 	0xD
#define PCI_HEADER_TYPE_OFFSET 		0xE
#define PCI_BIST_OFFSET 			0xF

#define PCIH0_BAR0_OFFSET   0x10
#define PCIH0_BAR1_OFFSET   0x14
#define PCIH0_BAR2_OFFSET   0x18
#define PCIH0_BAR3_OFFSET   0x1C
#define PCIH0_BAR4_OFFSET   0x20
#define PCIH0_BAR5_OFFSET   0x24

#include <kot/sys.h>
#include <kot/arch.h>
#include <kot/heap.h>
#include <kot/utils.h>
#include <kot/cstring.h>
#include <kot/uisd/srvs/pci.h>
#include <kot/uisd/srvs/system.h>

#include <srv/srv.h>
#include <pci/pci.h>
#include <pcie/pcie.h>

struct PCIDeviceHeader{
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
}__attribute__((packed));

struct PCIHeader0{
    PCIDeviceHeader Header;
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
}__attribute__((packed));

struct PCIHeader1{
    PCIDeviceHeader Header;
    /* TODO */
}__attribute__((packed));

struct PCIBar{
    uint64_t Base;
    uint64_t Size;
    uint8_t Type;
};

struct PCIDevice_t{
    PCIDeviceHeader* Header;
    PCIBar* BAR[6];
    uint64_t BARNum;
    PCIDeviceID_t Index;
};

extern process_t proc;

extern PCIDevice_t** PCIDevices;
extern PCIDeviceID_t PCIDevicesIndex;

bool CheckDevice(PCIDeviceID_t device);
PCIDevice_t* GetDevice(PCIDeviceID_t device);
uint64_t Search(uint16_t vendorID, uint16_t deviceID, uint16_t subClassID, uint16_t classID, uint16_t progIF);
PCIDeviceID_t GetDevice(uint16_t vendorID, uint16_t deviceID, uint16_t subClassID, uint16_t classID, uint16_t progIF, PCIDeviceID_t index);