#pragma once
#include <core/main.h>
#include <controller/controller.h>

#define ATA_FIS_DRQ                     1 << 3 // Data transfert resquested
#define ATA_DEV_BUSY                    1 << 7  // Port busy

#define HBA_INTERRUPT_STATU_TFE         1 << 30 // Task File Error
#define HBA_COMMAND_LIST_MAX_ENTRIES    32

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
    Packet          = 0xA1,
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

class Port {
    public:
        Port(class AHCIController* Parent, struct HBAPort_t* Port, PortTypeEnum Type, uint8_t Index);
        ~Port();

        void StopCMD();
        void StartCMD();

        KResult Read(uint64_t Sector, uint16_t SectorCount, uintptr_t Buffer);

        class AHCIController* Controller;
        struct HBAPort_t* HbaPort;
        PortTypeEnum PortType;
        uint8_t PortIndex;

        struct HBACommandHeader_t* CommandHeader;
        struct HBACommandTable_t* CommandAddressTable[HBA_COMMAND_LIST_MAX_ENTRIES];

        uintptr_t BufferVirtual;
        uintptr_t BufferPhysical;
        size64_t BufferSize;
};