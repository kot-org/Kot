#pragma once
#include <core/main.h>
#include <controller/controller.h>

#define ATA_SECTOR_SIZE                 0x200
#define ATA_FIS_DRQ                     1 << 3 // Data transfert resquested
#define ATA_DEV_BUSY                    1 << 7  // Port busy

#define HBA_INTERRUPT_STATU_TFE         1 << 30 // Task File Error
#define HBA_COMMAND_LIST_MAX_ENTRIES    0x20
#define HBA_PRDT_ENTRY_MAX_SIZE         0x2000
#define HBA_COMMAND_TABLE_SIZE          0x8000
#define HBA_PRDT_MAX_ENTRIES            (HBA_COMMAND_TABLE_SIZE - sizeof(HBACommandTable_t)) / sizeof(HBAPRDTEntry_t)
#define HBA_PRDT_ENTRY_ADDRESS_SIZE     0x1000
#define HBA_PRDT_ENTRY_SECTOR_SIZE      HBA_PRDT_ENTRY_ADDRESS_SIZE / ATA_SECTOR_SIZE

#define ATA_CMD_TIMEOUT                 1000000



enum PortTypeEnum{
    None            = 0x0,
    SATA            = 0x1,
    SEMB            = 0x2,
    PM              = 0x3,
    SATAPI          = 0x4,
};

enum ATACommandEnum{
    ReadPIO         = 0x20,
    ReadDMA         = 0x25,
    WritePIO        = 0x30,
    WriteDMA        = 0x35,
    Packet          = 0xA0,
    Flush           = 0xE7,
    Identify        = 0xEC
};

enum FISTypeEnum{
    HostToDevice    = 0x27,
    DeviceToHost    = 0x34,
    DMA_ACT         = 0x39,
    DMA_SETUP       = 0x41,
    Data            = 0x46,
    BIST            = 0x58,
    PIO_SETUP       = 0x5F,
    DEV_BITS        = 0xA1,
};

struct HBACommandHeader_t{
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
    uint64_t CommandTableBaseAddress;
    uint32_t Reserved1[4];
}__attribute__((packed));

struct HBAPRDTEntry_t{
    uint64_t DataBaseAddress;
    uint32_t Reserved0;
    uint32_t ByteCount:22;
    uint32_t Reserved1:9;
    uint32_t InterruptOnCompletion:1;
};

struct HBACommandTable_t{
    uint8_t CommandFIS[64];
    uint8_t AtapiCommand[16];
    uint8_t Reserved[48];
    HBAPRDTEntry_t PrdtEntry[];
};

struct FisHostToDeviceRegisters_t{
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
}__attribute__((packed));

/* https://www.seagate.com/www-content/product-content/seagate-laptop-fam/barracuda_25/en-us/docs/100804767b.pdf 4.3.1 */
struct IdentifyInfo_t{
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

class Port{
    public:
        Port(class AHCIController* Parent, struct HBAPort_t* Port, PortTypeEnum Type, uint8_t Index);
        ~Port();

        void StopCMD();
        void StartCMD();

        KResult Read(uint64_t Start, size64_t Size);
        KResult Write(uint64_t Start, size64_t Size);

        uint64_t GetSize();
        uint16_t* GetModelNumber();
        uint16_t* GetSerialNumber();

        class AHCIController* Controller;
        struct HBAPort_t* HbaPort;
        PortTypeEnum PortType;
        uint8_t PortIndex;

        struct HBACommandHeader_t* CommandHeader;
        struct HBACommandTable_t* CommandAddressTable[HBA_COMMAND_LIST_MAX_ENTRIES];

        ksmem_t BufferKey;
        uintptr_t BufferVirtual;
        size64_t BufferSize;

        IdentifyInfo_t* IdentifyInfo;

        uint64_t Lock;
    private:
        KResult GetIdentifyInfo();
};