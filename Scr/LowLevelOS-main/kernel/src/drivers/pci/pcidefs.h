#pragma once
#include <stdint.h>

struct PCIDevice {
    uint16_t VendorID;
    uint16_t DeviceID;
    uint16_t Command;
    uint16_t Status;
    uint8_t RevisionID;
    uint8_t ProgramInterface;
    uint8_t Subclass;
    uint8_t Class;
    uint8_t CacheLineSize;
    uint8_t LatencyTimer;
    uint8_t HeaderType;
    uint8_t BIST;
};

struct PCIHeader0 {
    PCIDevice Header;
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
    uint16_t Rsv0;
    uint16_t Rsv1;
    uint16_t Rsv2;
    uint8_t IntreruptLine;
    uint8_t IntreruptPin;
    uint8_t MinGrant;
    uint8_t MaxLatency;
};


struct TranslatedPCIDevice {
    const char* VendorID;
    const char* DeviceID;
    const char* Subclass;
    const char* Class;    
    uint16_t Command;
    uint16_t Status;
    uint8_t RevisionID;
    uint8_t ProgramInterface;
    uint8_t CacheLineSize;
    uint8_t LatencyTimer;
    uint8_t HeaderType;
    uint8_t BIST;
};