#pragma once

#include "../../lib/types.h"
#include "../../hardware/pci/pci.h"
#include "../../lib/lib.h"
#include "../../lib/stdio.h"
#include "../../memory/paging/pageTableManager.h"
#include "../../memory/paging/pageFrameAllocator.h"
#include "../../memory/heap/heap.h"



namespace AHCI{
    #define ATA_DEV_BUSY 0x80
    #define ATA_DEV_DRQ 0x08

    #define HBA_PxIS_TFES (1 << 30)

    enum sata_command {
        ATA_CMD_READ_PIO = 0x20,
        ATA_CMD_READ_DMA_EX = 0x25,
        ATA_CMD_WRITE_PIO = 0x30,
        ATA_CMD_WRITE_DMA_EX = 0x35,
        ATA_CMD_PACKET = 0xA1,
        ATA_CMD_FLUSH = 0xE7,
        ATA_CMD_IDENTIFY = 0xEC
    };

    enum PortType {
        None = 0,
        SATA = 1,
        SEMB = 2,
        PM = 3,
        SATAPI = 4,
    };

    enum FIS_TYPE{
        FIS_TYPE_REG_H2D = 0x27,
        FIS_TYPE_REG_D2H = 0x34,
        FIS_TYPE_DMA_ACT = 0x39,
        FIS_TYPE_DMA_SETUP = 0x41,
        FIS_TYPE_DATA = 0x46,
        FIS_TYPE_BIST = 0x58,
        FIS_TYPE_PIO_SETUP = 0x5F,
        FIS_TYPE_DEV_BITS = 0xA1,
    };

    struct HBAPort{
        uint32_t CommandListBase;
        uint32_t CommandListBaseUpper;
        uint32_t FisBaseAddress;
        uint32_t FisBaseAddressUpper;
        uint32_t InterruptStatus;
        uint32_t InterruptEnable;
        uint32_t CommandStatus;
        uint32_t Reserved0;
        uint32_t TaskFileData;
        uint32_t Signature;
        uint32_t SataStatus;
        uint32_t SataControl;
        uint32_t SataError;
        uint32_t SataActive;
        uint32_t CommandIssue;
        uint32_t SataNotification;
        uint32_t FisSwitchControl;
        uint32_t Reserved1[11];
        uint32_t Vendor[4];
    };

    struct HBAMemory{
        uint32_t HostCapability;
        uint32_t GlobalHostControl;
        uint32_t InterruptStatus;
        uint32_t PortsImplemented;
        uint32_t Version;
        uint32_t CccControl;
        uint32_t CccPorts;
        uint32_t EnclosureManagementLocation;
        uint32_t EnclosureManagementControl;
        uint32_t HostCapabilitiesExtended;
        uint32_t BiosHandoffCtrlSts;
        uint8_t Reserved0[0x74];
        uint8_t Vendor[0x60];
        HBAPort Ports[1];
    };

    struct HBACommandHeader {
        uint8_t CommandFISLength:5;
        uint8_t Atapi:1;
        uint8_t Write:1;
        uint8_t Prefetchable:1;

        uint8_t Reset:1;
        uint8_t Bist:1;
        uint8_t ClearBusy:1;
        uint8_t Reserved0:1;
        uint8_t PortMultiplier:4;

        uint16_t PrdtLength;
        uint32_t PrdbCount;
        uint32_t CommandTableBaseAddress;
        uint32_t CommandTableBaseAddressUpper;
        uint32_t Reserved1[4];
    };

    struct HBAPRDTEntry{
        uint64_t DataBaseAddress;
        uint32_t Reserved0;

        uint32_t ByteCount:22;
        uint32_t Reserved1:9;
        uint32_t InterruptOnCompletion:1;
    };

    struct HBACommandTable{
        uint8_t CommandFIS[64];

        uint8_t AtapiCommand[16];

        uint8_t Reserved[48];

        HBAPRDTEntry PrdtEntry[];
    };

    /* https://www.seagate.com/www-content/product-content/seagate-laptop-fam/barracuda_25/en-us/docs/100804767b.pdf 4.3.1 */
    struct ATACommandIdentify{
        uint16_t ConfigInfo;
        uint16_t NumberLogicalCylinders;
        uint16_t SpecificConfigurations;
        uint16_t NumberLogicalHead;
        uint16_t Reserved0;
        uint16_t Reserved1;
        uint16_t NumberSectorPerTrack;
        uint16_t Reserved2[3]; /* size + 1 */
        uint16_t SerialNumber[10];
        uint16_t Reserved3;
        uint16_t Reserved4;
        uint16_t Reserved5; //Obsolete
        uint16_t FirmwareRevision[4];
        uint16_t DriveModelNumber[20];
        uint16_t MaximumSectorPerInterrupt; //Maximum sectors per interrupt on Read multiple and Write multiple 
        uint16_t TrustedComputingFeatureSetOptions;
        uint16_t StandardStandbyTimer;
        uint16_t Capabilities;                      /* offset 50*/
        uint16_t PIODataTransferCycleTimingMode;
        uint16_t Reserved6;
        uint16_t CheckValidity;
        uint16_t CurrentLogicalCylinders;
        uint16_t CurrentLogicalHeads;
        uint16_t CurrentLogicalSectorsPerLogicalTrack;
        uint32_t CurrentCapacitySectors;
        uint16_t SectorsTransferredDuringReadMultiple;
        uint16_t TotalNumberUserAddressableSectors[2];
        uint16_t Reserved7;
        uint16_t MultiwordDMAActiveModes;
        uint16_t PIOModeSupported; 
        uint16_t MinDMATransferCycleTimePerWord; //minimal
        uint16_t RecDMATransferCycleTimePerWord; //recommended
        uint16_t MinimumPIOCycleTimeWithoutIORDY;
        uint16_t MinimumPIOCycletimeWithIORDY;
        uint16_t AdditionalSupportedBits;
        uint16_t ATAReserved0[5];
        uint16_t QueueDepth;
        uint16_t SerialATACapabilities;
        uint16_t ATAReserved1;
        uint16_t SerialATAFeaturesSupported;
        uint16_t SerialATAFeaturesEnabled;
        uint16_t MajorVersionNumber;
        uint16_t MinorVersionNumber;
        uint16_t CommandSetsSupported0;
        uint16_t CommandSetsSupported1;
        uint16_t CommandSetsSupportExtension;
        uint16_t CommandSetsEnabled0;
        uint16_t CommandSetsEnabled1;
        uint16_t CommandSsetsEnableExtension;
        uint16_t DMACurrentMode;
        uint16_t SecurityEraseTime;
        uint16_t EnhancedSecurityEraseTime; /* 90 verify */
        uint16_t CurrentAPMValues;
        uint16_t MasterPasswordRevisionCode;
        uint16_t HardwareResetValue;
        uint16_t AutoAcousticManagementSetting;
        uint16_t StreamMinRequestSizeDMA; 
        uint16_t StreamingTransferTimeDMA; 
        uint16_t StreamingAccessLatency;
        uint32_t StreamingPerformanceGranularity;
        uint64_t TotalNumberUserAddressableLBASectorsAvailable;
        uint16_t StreamingTransferTimePIO;
        uint16_t Reserved8;
        uint16_t SectorSize; //physical and logical
        uint16_t Reserved9;
        uint64_t DriveWWN; //The mandatory value of the world wide name
        uint16_t ATAReserved2[7];
        uint16_t FreeFallProtectionSupport:5;
        uint16_t FreeFallProtectionEnableOrDisable:5;
        uint16_t ATAReserved3[7];
        uint16_t SecurityStatus;
        uint16_t Reserved10[31];
        uint16_t ATAReserved4[62];
        uint16_t TransportMajorVersionNumber;
        uint16_t ATAReserved5[32];
        uint16_t IntegrityWord;
    }__attribute__((packed));

    struct FIS_REG_H2D {
        uint8_t FisType;

        uint8_t PortMultiplier:4;
        uint8_t Reserved0:3;
        uint8_t CommandControl:1;

        uint8_t Command;
        uint8_t FeatureLow;

        uint8_t Lba0;
        uint8_t Lba1;
        uint8_t Lba2;
        uint8_t DeviceRegister;

        uint8_t Lba3;
        uint8_t Lba4;
        uint8_t Lba5;
        uint8_t FeatureHigh;

        uint8_t CountLow;
        uint8_t CountHigh;
        uint8_t IsoCommandCompletion;
        uint8_t Control;

        uint8_t Reserved1[4];
    };

    struct GUIDPartitionEntryFormat{
        GUID PartitionTypeGUID;
        GUID UniquePartitionGUID;
        uint64_t FirstLBA;
        uint64_t LastLBA;
        uint64_t Flags;
        uint16_t PartitionName[36];
    }__attribute__((packed));

    
    class Port {
        public:
            ATACommandIdentify* DiskInfo;
            HBAPort* HbaPort;
            PortType portType;
            void* Buffer; //the size of the buffer is 0x1000
            uint64_t BufferSize;
            uint8_t PortNumber;
            void Configure();
            void StartCMD();
            void StopCMD();
            bool Read(uint64_t sector, uint16_t sectorCount, void* buffer);
            bool Write(uint64_t sector, uint16_t sectorCount, void* buffer);
            bool GetDiskInfo();
            uint64_t GetNumberSectorsLBA(); //LBA sectors is 512 bytes
            uint64_t GetSectorNumberPhysical();
            uint16_t GetSectorSizeLBA();
            uint16_t GetSectorSizePhysical();            
            uint64_t GetSize();
            uint16_t* GetModelNumber();
            uint16_t* GetSerialNumber();    
            void ResetDisk();    
            bool IsPortInit(GUID* nameOfInitPartition);    
            bool IsPortSystem(GUID* GUIDOfSystemPartition);           
    };

    struct PartitionInfo{
        Port* port;
        GUIDPartitionEntryFormat* PartitionInfo;
        char* PartitionName;
        char* FSSignature;
        void* FSData;
    };

    struct PartitionNode{
        PartitionInfo Content;
        PartitionNode* Last;
        PartitionNode* Next;
    };

    class AHCIDriver{
        public:
            AHCIDriver(PCI::PCIDeviceHeader* pciBaseAddress);
            ~AHCIDriver();
            PartitionInfo* GetSystemPartition();
            PCI::PCIDeviceHeader* PCIBaseAddress;
            HBAMemory* ABAR;
            void ProbePorts();
            Port* Ports[32];
            uint8_t PortCount;
            PartitionNode* PartitionsList;
    };

    extern AHCIDriver* ahciDriver;
}