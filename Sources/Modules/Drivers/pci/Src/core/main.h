#pragma once

#define PCI_CONFIG_ADDR     0xCF8
#define PCI_CONFIG_DATA     0xCFC

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

#include <kot/sys.h>
#include <kot/arch.h>
#include <kot/heap.h>
#include <kot/cstring.h>

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
    uint32_t BAR0;
    uint32_t BAR1;
    uint32_t BAR2;
    uint32_t BAR3;
    uint32_t BAR4;
    uint32_t BAR5;
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
