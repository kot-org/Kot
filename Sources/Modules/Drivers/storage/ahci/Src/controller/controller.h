#pragma once
#include <core/main.h>
#include <port/port.h>

#define HBA_PORT_IPM_ACTIVE 0x1
#define HBA_PORT_DEV_PRESENT 0x3
#define SATA_SIG_ATAPI 0xEB140101
#define SATA_SIG_ATA 0x00000101
#define SATA_SIG_SEMB 0xC33C0101
#define SATA_SIG_PM 0x96690101

#define HBA_PxCMD_CR 0x8000
#define HBA_PxCMD_FRE 0x0010
#define HBA_PxCMD_ST 0x0001
#define HBA_PxCMD_FR 0x4000


enum PortType {
    None = 0,
    SATA = 1,
    SEMB = 2,
    PM = 3,
    SATAPI = 4,
};

struct HBAMemory_t{
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
    uint8_t Reserved0[(0xA0 - 0x2C)];
    uint8_t Vendor[(0x100 - 0xA0)];
    HBAPort_t Ports[];
};


class AHCIController{
    public:
        AHCIController(srv_pci_bar_info_t* BarInfo);
        ~AHCIController();
        void FindPorts();
        HBAMemory_t* ABAR;
};