#include <controller/controller.h>

AHCIController::AHCIController(srv_pci_bar_info_t* BarInfo){
    ABAR = (HBAMemory_t*)MapPhysical(BarInfo->Address, BarInfo->Size);
    FindPorts();
}

AHCIController::~AHCIController(){

}

PortTypeEnum GetPortType(HBAPort_t* port){
    uint32_t sataStatus = port->SataStatus;

    uint8_t interfacePowerManagement = (sataStatus >> 8) & 0b111;
    uint8_t deviceDetection = sataStatus & 0b111;

    if(deviceDetection != HBA_PORT_DEV_PRESENT) return PortTypeEnum::None;
    if(interfacePowerManagement != HBA_PORT_IPM_ACTIVE) return PortTypeEnum::None;

    switch (port->Signature){
        case SATA_SIG_ATAPI:
            return PortTypeEnum::SATAPI;
        case SATA_SIG_ATA:
            return PortTypeEnum::SATA;
        case SATA_SIG_PM:
            return PortTypeEnum::PM;
        case SATA_SIG_SEMB:
            return PortTypeEnum::SEMB;
        default:
            PortTypeEnum::None;
    }
}

void AHCIController::FindPorts(){
    // set port find to 0
    PortCount = 0;
    uint32_t PortsImplemented = ABAR->PortsImplemented;
    for (uint8_t i = 0; i < PORT_MAX_COUNT; i++){
        if (PortsImplemented & (1 << i)){ // check if port is implemented
            PortTypeEnum PortType = GetPortType(&ABAR->Ports[i]);
            if (PortType == PortTypeEnum::SATA || PortType == PortTypeEnum::SATAPI){
                Ports[PortCount] = new Port(this, &ABAR->Ports[i], PortType, PortCount);
                PortCount++;
            }
        }
    }
}