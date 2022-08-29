#include <controller/controller.h>

AHCIController::AHCIController(srv_pci_bar_info_t* BarInfo){
    ABAR = (HBAMemory_t*)MapPhysical(BarInfo->Address, BarInfo->Size);
    FindPorts();
}

AHCIController::~AHCIController(){

}

PortType GetPortType(HBAPort_t* port){
    uint32_t sataStatus = port->SataStatus;

    uint8_t interfacePowerManagement = (sataStatus >> 8) & 0b111;
    uint8_t deviceDetection = sataStatus & 0b111;

    if(deviceDetection != HBA_PORT_DEV_PRESENT) return PortType::None;
    if(interfacePowerManagement != HBA_PORT_IPM_ACTIVE) return PortType::None;

    switch (port->Signature){
        case SATA_SIG_ATAPI:
            return PortType::SATAPI;
        case SATA_SIG_ATA:
            return PortType::SATA;
        case SATA_SIG_PM:
            return PortType::PM;
        case SATA_SIG_SEMB:
            return PortType::SEMB;
        default:
            PortType::None;
    }
}

void AHCIController::FindPorts(){
    uint32_t PortsImplemented = ABAR->PortsImplemented;
    for (uint8_t i = 0; i < 32; i++){
        if (PortsImplemented & (1 << i)){ // check if port is implemented
            PortType PortType = GetPortType(&ABAR->Ports[i]);
            if (portType == PortType::SATA || portType == PortType::SATAPI){
                Ports[PortCount] = new Port();
                Ports[PortCount]->PortType = PortType;
                Ports[PortCount]->HbaPort = &ABAR->Ports[i];
                Ports[PortCount]->PortNumber = PortCount;
                PortCount++;
            }
            std::printf("Port found");
        }
    }
}