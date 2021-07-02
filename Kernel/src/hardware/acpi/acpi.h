#pragma once
#include <stdint.h>

namespace ACPI{
    struct RSDP2 {
        unsigned char Signature[8];
        uint8_t Checksum;
        uint8_t OEMID[6];
        uint8_t Revision;
        uint32_t RSDTAddress;
        uint32_t Length;
        uint64_t XSDTAddress;
        uint8_t ExtendedChecksum;
        uint8_t Reserved[3];
    } __attribute__((packed));

    struct SDTHeader{
        unsigned char Signature[4];
        uint32_t Length;
        uint8_t Revision;
        uint8_t Checksum;
        uint8_t OEMID[6];
        uint8_t OEMTableID[8];
        uint32_t OEMRevision;
        uint32_t CreatorID;
        uint32_t CreatorRevision;
    }__attribute__((packed));

    struct MCFGHeader{
        SDTHeader Header;
        uint64_t Reserved;
    }__attribute__((packed));

    struct MADTHeader{
        SDTHeader   Header;
        uint32_t    APICAddress;
        uint32_t    Flags;
    }__attribute__((packed));

    struct DeviceConfig{
        uint64_t BaseAddress;
        uint16_t PCISegGroup;
        uint8_t StartBus;
        uint8_t EndBus;
        uint32_t Reserved;
    }__attribute__((packed));

    struct GenericAddressStructure{
        uint8_t AddressSpace;
        uint8_t BitWidth;
        uint8_t BitOffset;
        uint8_t AccessSize;
        uint64_t Address;
    }__attribute__((packed));

    struct FADTHeader{
        SDTHeader Header;
        uint32_t FirmwareCtrl;
        uint32_t Dsdt;
        uint8_t  Reserved;
        uint8_t  PreferredPowerManagementProfile;
        uint16_t SCI_Interrupt;
        uint32_t SMI_CommandPort;
        uint8_t  AcpiEnable;
        uint8_t  AcpiDisable;
        uint8_t  S4BIOS_REQ;
        uint8_t  PSTATE_Control;
        uint32_t PM1aEventBlock;
        uint32_t PM1bEventBlock;
        uint32_t PM1aControlBlock;
        uint32_t PM1bControlBlock;
        uint32_t PM2ControlBlock;
        uint32_t PMTimerBlock;
        uint32_t GPE0Block;
        uint32_t GPE1Block;
        uint8_t  PM1EventLength;
        uint8_t  PM1ControlLength;
        uint8_t  PM2ControlLength;
        uint8_t  PMTimerLength;
        uint8_t  GPE0Length;
        uint8_t  GPE1Length;
        uint8_t  GPE1Base;
        uint8_t  CStateControl;
        uint16_t WorstC2Latency;
        uint16_t WorstC3Latency;
        uint16_t FlushSize;
        uint16_t FlushStride;
        uint8_t  DutyOffset;
        uint8_t  DutyWidth;
        uint8_t  DayAlarm;
        uint8_t  MonthAlarm;
        uint8_t  Century;
        uint16_t BootArchitectureFlags;
        uint8_t  Reserved2;
        GenericAddressStructure ResetReg;
        uint8_t  ResetValue;
        uint8_t  Reserved3[3];
        uint64_t X_FirmwareControl;
        uint64_t X_Dsdt;
        GenericAddressStructure X_PM1aEventBlock;
        GenericAddressStructure X_PM1bEventBlock;
        GenericAddressStructure X_PM1aControlBlock;
        GenericAddressStructure X_PM1bControlBlock;
        GenericAddressStructure X_PM2ControlBlock;
        GenericAddressStructure X_PMTimerBlock;
        GenericAddressStructure X_GPE0Block;
        GenericAddressStructure X_GPE1Block;
    }__attribute__((packed));

    struct SLPInfo{
        uint16_t TYPa;
        uint16_t TYPb;
        uint16_t EN;
    };

    void* FindTable(SDTHeader* sdtHeader, char* signature);
    void InitializeFADT(FADTHeader* fadt);
    void Restart();
    void Shutdown();
}