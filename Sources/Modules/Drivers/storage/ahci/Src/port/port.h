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

struct FisDeviceToHostRegisters_t{
    uint8_t FisType;

    uint8_t PortMultiplier:4;

    uint8_t Reserved0:2;
    uint8_t Interrupt:1;
    uint8_t Reserved1:1;
    uint8_t Status;
    uint8_t Error;

    uint8_t Lba0;
    uint8_t Lba1;
    uint8_t Lba2;
    uint8_t DeviceRegister;

    uint8_t Lba3;
    uint8_t Lba4;
    uint8_t Lba5;
    uint8_t Reserved2;

    uint8_t CountLow;
    uint8_t CountHigh;
    uint8_t Reserved3[4];
}__attribute__((packed));

/* Identify Info */
struct IdentifyInfo_GeneralConfiguration_t{
    uint8_t Reserved0:2;
    uint8_t ResponseIncomplete:1;
    uint8_t Reserved1:3;
    uint8_t FixedDevice:1;
    uint8_t RemovableMedia:1;
    uint8_t Reserved2:7;
    uint8_t DeviceType:1;        
}__attribute__((packed));

struct IdentifyInfo_TrustedComputing_t{
    uint8_t TrustedComputingFeatureSetOptions:1;
    uint16_t Reserved0:15;
}__attribute__((packed));

struct IdentifyInfo_Capabilities_t{
    uint8_t  CurrentLongPhysicalSectorAlignment:2;
    uint8_t  Reserved0:6;
    uint8_t  DmaSupported:1;
    uint8_t  LbaSupported:1;
    uint8_t  IordyDisable:1;
    uint8_t  IordySupported:1;
    uint8_t  Reserved1:1;
    uint8_t  StandybyTimerSupport:1;
    uint8_t  Reserved2:2;
    uint16_t Reserved3;
}__attribute__((packed));

struct IdentifyInfo_AdditionalSupported_t{
    uint16_t ZonedCapabilities:2;
    uint16_t NonVolatileWriteCache:1;
    uint16_t ExtendedUserAddressableSectorsSupported:1;
    uint16_t DeviceEncryptsAllUserData:1;
    uint16_t ReadZeroAfterTrimSupported:1;
    uint16_t Optional28BitCommandsSupported:1;
    uint16_t IEEE1667:1;
    uint16_t DownloadMicrocodeDmaSupported:1;
    uint16_t SetMaxSetPasswordUnlockDmaSupported:1;
    uint16_t WriteBufferDmaSupported:1;
    uint16_t ReadBufferDmaSupported:1;
    uint16_t DeviceConfigIdentifySetDmaSupported:1;
    uint16_t LPSAERCSupported:1;
    uint16_t DeterministicReadAfterTrimSupported:1;
    uint16_t CFastSpecSupported:1;
}__attribute__((packed));

struct IdentifyInfo_SerialAtaCapabilities_t{
    uint16_t Reserved0:1;
    uint16_t SataGen1:1;
    uint16_t SataGen2:1;
    uint16_t SataGen3:1;
    uint16_t Reserved1:4;
    uint16_t NCQ:1;
    uint16_t HIPM:1;
    uint16_t PhyEvents:1;
    uint16_t NcqUnload:1;
    uint16_t NcqPriority:1;
    uint16_t HostAutoPS:1;
    uint16_t DeviceAutoPS:1;
    uint16_t ReadLogDMA:1;
    uint16_t Reserved2:1;
    uint16_t CurrentSpeed:3;
    uint16_t NcqStreaming:1;
    uint16_t NcqQueueMgmt:1;
    uint16_t NcqReceiveSend:1;
    uint16_t DEVSLPtoReducedPwrState:1;
    uint16_t Reserved3:8;
}__attribute__((packed));

struct IdentifyInfo_SerialAtaFeaturesSupported_t{
    uint16_t Reserved0:1;
    uint16_t NonZeroOffsets:1;
    uint16_t DmaSetupAutoActivate:1;
    uint16_t DIPM:1;
    uint16_t InOrderData:1;
    uint16_t HardwareFeatureControl:1;
    uint16_t SoftwareSettingsPreservation:1;
    uint16_t NCQAutosense:1;
    uint16_t DEVSLP:1;
    uint16_t HybridInformation:1;
    uint16_t Reserved1:6;
}__attribute__((packed));

struct IdentifyInfo_SerialAtaFeaturesEnabled_t{
    uint16_t Reserved0:1;
    uint16_t NonZeroOffsets:1;
    uint16_t DmaSetupAutoActivate:1;
    uint16_t DIPM:1;
    uint16_t InOrderData:1;
    uint16_t HardwareFeatureControl:1;
    uint16_t SoftwareSettingsPreservation:1;
    uint16_t DeviceAutoPS:1;
    uint16_t DEVSLP:1;
    uint16_t HybridInformation:1;
    uint16_t Reserved1:6;
}__attribute__((packed));

struct IdentifyInfo_CommandSetSupport_t{
    uint16_t SmartCommands:1;
    uint16_t SecurityMode:1;
    uint16_t RemovableMediaFeature:1;
    uint16_t PowerManagement:1;
    uint16_t Reserved1:1;
    uint16_t WriteCache:1;
    uint16_t LookAhead:1;
    uint16_t ReleaseInterrupt:1;
    uint16_t ServiceInterrupt:1;
    uint16_t DeviceReset:1;
    uint16_t HostProtectedArea:1;
    uint16_t Obsolete1:1;
    uint16_t WriteBuffer:1;
    uint16_t ReadBuffer:1;
    uint16_t Nop:1;
    uint16_t Obsolete2:1;
    uint16_t DownloadMicrocode:1;
    uint16_t DmaQueued:1;
    uint16_t Cfa:1;
    uint16_t AdvancedPm:1;
    uint16_t Msn:1;
    uint16_t PowerUpInStandby:1;
    uint16_t ManualPowerUp:1;
    uint16_t Reserved2:1;
    uint16_t SetMax:1;
    uint16_t Acoustics:1;
    uint16_t BigLba:1;
    uint16_t DeviceConfigOverlay:1;
    uint16_t FlushCache:1;
    uint16_t FlushCacheExt:1;
    uint16_t WordValid83:2;
    uint16_t SmartErrorLog:1;
    uint16_t SmartSelfTest:1;
    uint16_t MediaSerialNumber:1;
    uint16_t MediaCardPassThrough:1;
    uint16_t StreamingFeature:1;
    uint16_t GpLogging:1;
    uint16_t WriteFua:1;
    uint16_t WriteQueuedFua:1;
    uint16_t WWN64Bit:1;
    uint16_t URGReadStream:1;
    uint16_t URGWriteStream:1;
    uint16_t ReservedForTechReport:2;
    uint16_t IdleWithUnloadFeature:1;
    uint16_t WordValid:2;
}__attribute__((packed));

struct IdentifyInfo_CommandSetActive_t{
    uint16_t SmartCommands:1;
    uint16_t SecurityMode:1;
    uint16_t RemovableMediaFeature:1;
    uint16_t PowerManagement:1;
    uint16_t Reserved1:1;
    uint16_t WriteCache:1;
    uint16_t LookAhead:1;
    uint16_t ReleaseInterrupt:1;
    uint16_t ServiceInterrupt:1;
    uint16_t DeviceReset:1;
    uint16_t HostProtectedArea:1;
    uint16_t Obsolete1:1;
    uint16_t WriteBuffer:1;
    uint16_t ReadBuffer:1;
    uint16_t Nop:1;
    uint16_t Obsolete2:1;
    uint16_t DownloadMicrocode:1;
    uint16_t DmaQueued:1;
    uint16_t Cfa:1;
    uint16_t AdvancedPm:1;
    uint16_t Msn:1;
    uint16_t PowerUpInStandby:1;
    uint16_t ManualPowerUp:1;
    uint16_t Reserved2:1;
    uint16_t SetMax:1;
    uint16_t Acoustics:1;
    uint16_t BigLba:1;
    uint16_t DeviceConfigOverlay:1;
    uint16_t FlushCache:1;
    uint16_t FlushCacheExt:1;
    uint16_t Resrved3:1;
    uint16_t Words119_120Valid:1;
    uint16_t SmartErrorLog:1;
    uint16_t SmartSelfTest:1;
    uint16_t MediaSerialNumber:1;
    uint16_t MediaCardPassThrough:1;
    uint16_t StreamingFeature:1;
    uint16_t GpLogging:1;
    uint16_t WriteFua:1;
    uint16_t WriteQueuedFua:1;
    uint16_t WWN64Bit:1;
    uint16_t URGReadStream:1;
    uint16_t URGWriteStream:1;
    uint16_t ReservedForTechReport:2;
    uint16_t IdleWithUnloadFeature:1;
    uint16_t Reserved4:2;
}__attribute__((packed));

struct IdentifyInfo_NormalSecurityEraseUnit_t{
    uint16_t TimeRequired:15;
    uint16_t ExtendedTimeReported:1;
}__attribute__((packed));

struct IdentifyInfo_EnhancedSecurityEraseUnit_t{
    uint16_t TimeRequired:15;
    uint16_t ExtendedTimeReported:1;
}__attribute__((packed));

struct IdentifyInfo_PhysicalLogicalSectorSize_t{
    uint16_t LogicalSectorsPerPhysicalSector:4;
    uint8_t Reserved0;
    uint16_t LogicalSectorLongerThan512Bytes:1;
    uint16_t MultipleLogicalSectorsPerPhysicalSector:1;
    uint16_t Reserved1:2;
}__attribute__((packed));

struct IdentifyInfo_CommandSetSupportExt_t{
    uint16_t ReservedForDrqTechnicalReport:1;
    uint16_t WriteReadVerify:1;
    uint16_t WriteUncorrectableExt:1;
    uint16_t ReadWriteLogDmaExt:1;
    uint16_t DownloadMicrocodeMode3:1;
    uint16_t FreefallControl:1;
    uint16_t SenseDataReporting:1;
    uint16_t ExtendedPowerConditions:1;
    uint16_t Reserved0:6;
    uint16_t WordValid:2;
}__attribute__((packed));

struct IdentifyInfo_CommandSetActiveExt_t{
    uint16_t ReservedForDrqTechnicalReport:1;
    uint16_t WriteReadVerify:1;
    uint16_t WriteUncorrectableExt:1;
    uint16_t ReadWriteLogDmaExt:1;
    uint16_t DownloadMicrocodeMode3:1;
    uint16_t FreefallControl:1;
    uint16_t SenseDataReporting:1;
    uint16_t ExtendedPowerConditions:1;
    uint16_t Reserved0:6;
    uint16_t Reserved1:2;
}__attribute__((packed));

struct IdentifyInfo_SecurityStatus_t{
    uint16_t SecuritySupported:1;
    uint16_t SecurityEnabled:1;
    uint16_t SecurityLocked:1;
    uint16_t SecurityFrozen:1;
    uint16_t SecurityCountExpired:1;
    uint16_t EnhancedSecurityEraseSupported:1;
    uint16_t Reserved0:2;
    uint16_t SecurityLevel:1;
    uint16_t Reserved1:7;
}__attribute__((packed));

struct IdentifyInfo_CfaPowerMode1_t{
    uint16_t MaximumCurrentInMA:12;
    uint16_t CfaPowerMode1Disabled:1;
    uint16_t CfaPowerMode1Required:1;
    uint16_t Reserved0:1;
    uint16_t Word160Supported:1;
}__attribute__((packed));

struct IdentifyInfo_DataSetManagementFeature_t{
    uint16_t SupportsTrim:1;
    uint16_t Reserved0:15;
}__attribute__((packed));

struct IdentifyInfo_SCTCommandTransport_t{
    uint16_t Supported:1;
    uint16_t Reserved0:1;
    uint16_t WriteSameSuported:1;
    uint16_t ErrorRecoveryControlSupported:1;
    uint16_t FeatureControlSuported:1;
    uint16_t DataTablesSuported:1;
    uint16_t Reserved1:6;
    uint16_t VendorSpecific:4;
}__attribute__((packed));

struct IdentifyInfo_BlockAlignment_t{
    uint16_t AlignmentOfLogicalWithinPhysical:14;
    uint16_t Word209Supported:1;
    uint16_t Reserved0:1;
}__attribute__((packed));

struct IdentifyInfo_NVCacheCapabilities_t{
    uint16_t NVCachePowerModeEnabled:1;
    uint16_t Reserved0:3;
    uint16_t NVCacheFeatureSetEnabled:1;
    uint16_t Reserved1:3;
    uint16_t NVCachePowerModeVersion:4;
    uint16_t NVCacheFeatureSetVersion:4;
}__attribute__((packed));

struct IdentifyInfo_NVCacheOptions_t{
    uint8_t NVCacheEstimatedTimeToSpinUpInSeconds;
    uint8_t Reserved;
}__attribute__((packed));

struct IdentifyInfo_TransportMajorVersion_t{
    uint16_t MajorVersion:12;
    uint16_t TransportType:4;
}__attribute__((packed));

struct IdentifyInfo_t{
    struct IdentifyInfo_GeneralConfiguration_t GeneralConfiguration;
    uint16_t NumberLogicalCylinders;
    uint16_t SpecificConfigurations;
    uint16_t NumberLogicalHead;
    uint16_t Reserved0[2];
    uint16_t NumberSectorPerTrack;
    uint16_t VendorUnique0[3]; 
    uint8_t SerialNumber[20];
    uint16_t Reserved1[2];
    uint16_t Reserved2; //Obsolete
    uint8_t FirmwareRevision[8];
    uint8_t DriveModelNumber[40];
    uint8_t MaximumBlockTransfer; //Maximum sectors per interrupt on Read multiple and Write multiple 
    uint8_t VendorUnique1;
    struct IdentifyInfo_TrustedComputing_t TrustedComputing;
    struct IdentifyInfo_Capabilities_t Capabilities;
    uint16_t Reserved3[2];
    uint8_t TranslationFieldsValid:3;
    uint8_t Reserved4:3;
    uint8_t FreeFallControlSensitivity;
    uint16_t NumberOfCurrentCylinders;
    uint16_t NumberOfCurrentHeads;
    uint16_t CurrentSectorsPerTrack;
    uint32_t CurrentSectorCapacity;
    uint8_t CurrentMultiSectorSetting;
    uint8_t MultiSectorSettingValid:1;
    uint8_t Reserved5:3;
    uint8_t SanitizeFeatureSupported:1;
    uint8_t CryptoScrambleExtCommandSupported:1;
    uint8_t OverwriteExtCommandSupported:1;
    uint8_t BlockEraseExtCommandSupported:1;
    uint32_t UserAddressableSectors;
    uint16_t Reserved6;
    uint8_t MultiWordDMASupport;
    uint8_t MultiWordDMAActive;
    uint8_t AdvancedPIOModes;
    uint8_t Reserved7;
    uint16_t MinimumMWXferCycleTime;
    uint16_t RecommendedMWXferCycleTime;
    uint16_t MinimumPIOCycleTime;
    uint16_t MinimumPIOCycleTimeIORDY;
    struct IdentifyInfo_AdditionalSupported_t AdditionalSupported;
    uint16_t Reserved8[5];
    uint16_t QueueDepth:5;
    uint16_t Reserved9:11;
    struct IdentifyInfo_SerialAtaCapabilities_t SerialAtaCapabilities;
    struct IdentifyInfo_SerialAtaFeaturesSupported_t SerialAtaFeaturesSupported;
    struct IdentifyInfo_SerialAtaFeaturesEnabled_t SerialAtaFeaturesEnabled;
    uint16_t MajorRevision;
    uint16_t MinorRevision;
    struct IdentifyInfo_CommandSetSupport_t CommandSetSupport;
    struct IdentifyInfo_CommandSetActive_t CommandSetActive;
    uint8_t UltraDMASupport;
    uint8_t UltraDMAActive;
    struct IdentifyInfo_NormalSecurityEraseUnit_t NormalSecurityEraseUnit;
    struct IdentifyInfo_EnhancedSecurityEraseUnit_t EnhancedSecurityEraseUnit;
    uint8_t CurrentAPMLevel;
    uint8_t Reserved10;
    uint16_t MasterPasswordID;
    uint16_t HardwareResetResult;
    uint8_t CurrentAcousticValue;
    uint8_t RecommendedAcousticValue;
    uint16_t StreamMinRequestSize;
    uint16_t StreamingTransferTimeDMA;
    uint16_t StreamingAccessLatencyDMAPIO;
    uint32_t StreamingPerfGranularity;
    uint32_t Max48BitLBA[2];
    uint16_t StreamingTransferTime;
    uint16_t DsmCap;
    struct IdentifyInfo_PhysicalLogicalSectorSize_t PhysicalLogicalSectorSize;
    uint16_t InterSeekDelay;
    uint16_t WorldWideName[8];
    uint16_t ReservedForTlcTechnicalReport;
    uint16_t WordsPerLogicalSector[2];
    struct IdentifyInfo_CommandSetSupportExt_t CommandSetSupportExt;
    struct IdentifyInfo_CommandSetActiveExt_t CommandSetActiveExt;
    uint16_t ReservedForExpandedSupportandActive[6];
    uint16_t MsnSupport:2;
    uint16_t Reserved11:14;
    struct IdentifyInfo_SecurityStatus_t SecurityStatus;
    uint16_t Reserved12[31];
    struct IdentifyInfo_CfaPowerMode1_t CfaPowerMode1;
    uint16_t ReservedForCfaWord161[7];
    uint16_t NominalFormFactor:4;
    uint16_t Reserved13:12;
    struct IdentifyInfo_DataSetManagementFeature_t DataSetManagementFeature;
    uint16_t AdditionalProductID[4];
    uint16_t ReservedForCfaWord174[2];
    uint16_t CurrentMediaSerialNumber[30];
    struct IdentifyInfo_SCTCommandTransport_t SCTCommandTransport;
    uint16_t Reserved14[2];
    struct IdentifyInfo_BlockAlignment_t BlockAlignment;
    uint16_t WriteReadVerifySectorCountMode3Only[2];
    uint16_t WriteReadVerifySectorCountMode2Only[2];
    struct IdentifyInfo_NVCacheCapabilities_t NVCacheCapabilities;
    uint16_t NVCacheSizeLSW;
    uint16_t NVCacheSizeMSW;
    uint16_t NominalMediaRotationRate;
    uint16_t Reserved15;
    struct IdentifyInfo_NVCacheOptions_t NVCacheOptions;
    uint8_t WriteReadVerifySectorCountMode;
    uint8_t Reserved16;
    uint16_t Reserved17;
    struct IdentifyInfo_TransportMajorVersion_t TransportMajorVersion;
    uint16_t TransportMinorVersion;
    uint16_t Reserved18[6];
    uint64_t ExtendedNumberOfUserAddressableSectors;
    uint16_t MinBlocksPerDownloadMicrocodeMode03;
    uint16_t MaxBlocksPerDownloadMicrocodeMode03;
    uint16_t Reserved19[19];
    uint8_t Signature;
    uint8_t CheckSum;
}__attribute__((packed));

class Device{
    public:
        Device(class AHCIController* Parent, struct HBAPort_t* Port, PortTypeEnum Type, uint8_t Index);
        ~Device();

        void StopCMD();
        void StartCMD();
        int8_t FindSlot();

        KResult Read(uint64_t Start, size64_t Size);
        KResult Write(uint64_t Start, size64_t Size);


        uint64_t GetSize();
        uint8_t* GetModelNumber();
        uint8_t* GetSerialNumber();

        class AHCIController* Controller;
        struct HBAPort_t* HbaPort;
        PortTypeEnum PortType;
        uint8_t PortIndex;
        int8_t MainSlot;

        struct HBACommandHeader_t* CommandHeader;
        struct HBACommandTable_t* CommandAddressTable[HBA_COMMAND_LIST_MAX_ENTRIES];

        ksmem_t BufferKey;
        uintptr_t BufferVirtual;
        size64_t BufferRealSize;
        size64_t BufferUsableSize;

        IdentifyInfo_t* IdentifyInfo;

        uint64_t Lock;
    private:
        KResult GetIdentifyInfo();
};