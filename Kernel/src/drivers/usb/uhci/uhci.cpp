#include "uhci.h"

namespace UHCI{
    UHCI_REGISTERS* UHCIRegisters;
    void* UHCIStack = NULL;

    void InitializeUHCI(PCI::PCIDeviceHeader* device){
        uint16_t BasePort;
        UHCIRegisters = (UHCI_REGISTERS*)((PCI::PCIHeader0*)device)->BAR4;
        BasePort = (uint16_t)(uint64_t)(void*)((PCI::PCIHeader0*)device)->BAR4;

        globalGraphics->Update();

        // Disable Legacy Support
        IoWrite8(BasePort + REG_LEGSUP, 0x8f00);

        // Disable interrupts
        IoWrite8(BasePort + REG_INTR, 0);

        // Clear status
        IoWrite8(BasePort + REG_STS, 0xffff);

        // Enable controller
        IoWrite8(BasePort + REG_CMD, CMD_RUN_STOP);
    }

    void PortClear(uint64_t port, uint16_t data)
    {
        uint64_t status = IoRead8(port);
        status &= ~PORT_RWC;
        status &= ~data;
        status |= PORT_RWC & data;
        IoWrite8(port, status);
    }      
        
    void PortSet(uint64_t port, uint16_t data)
    {
        uint64_t status = IoRead8(port);
        status |= data;
        status &= ~PORT_RWC;
        IoWrite8(port, status);
    }
}