#pragma once

#include <core/main.h>

class E1000Controller {
    private:
        uintptr_t memoryBase;
        uint32_t ioBase;
        uint8_t barType;

        bool eePromExists;
        void DetectEEProm();
        uint32_t EEPromRead(uint8_t addr);
        
        uint8_t MediaAccCtrl[6];
        bool CheckMAC();
        void InitMAC();

    public:
        E1000Controller(srv_pci_bar_info_t* BarInfo);

        void writeCmd(uint16_t addr, uint32_t value);
        uint32_t readCmd(uint16_t addr);

};