#pragma once

#include "../../lib/types.h"
#include "../../memory/heap/heap.h"
#include "../acpi/acpi.h"
#include "../../memory/paging/pageTableManager.h"
#include "../../drivers/graphics/graphics.h"
#include "../../lib/stdio/cstr.h"

namespace PCI{
    struct PCIDeviceHeader{
        uint16_t VendorID;
        uint16_t DeviceID;
        uint16_t Command;
        uint16_t Status;
        uint8_t RevisionID;
        uint8_t ProgIF; //program interface
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
        uint16_t ExpansionRomBaseAddr;
        uint16_t CapabilitiesPtr;
        uint16_t Reserved0;
        uint16_t Reserved1;
        uint16_t Reserved2;
        uint8_t IntreruptLine;
        uint8_t IntreruptPin;
        uint8_t MinGrant;
        uint8_t MaxLatency;
    }__attribute__((packed));

    struct PCIDeviceHeaderSearcher{
        uint16_t VendorID;
        uint16_t DeviceID;
        uint8_t RevisionID;
        uint8_t ProgIF; //program interface
        uint8_t Subclass;
        uint8_t Class;
    };


    struct PCINode{
        PCIDeviceHeader* device;
        PCINode* next;
    };
    
    void EnumeratePCI(ACPI::MCFGHeader* mcfg);
    void SavePCIDevice(PCIDeviceHeader* device);
    PCIDeviceHeader* GetPCIDevice(PCIDeviceHeaderSearcher* searcher);

    extern const char* DeviceClasses[];

    const char* GetVendorName(uint16_t vendorID);
    const char* GetDeviceName(uint16_t vendorID, uint16_t deviceID);
    const char* GetSubclassName(uint8_t classCode, uint8_t subclassCode);
    const char* GetProgIFName(uint8_t classCode, uint8_t subclassCode, uint8_t progIF);
}