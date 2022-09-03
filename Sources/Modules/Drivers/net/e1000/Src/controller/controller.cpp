#include <controller/controller.h>

#include <kot/arch.h>
#include <utils/mmio.h>

#include <kot++/printf.h>

using namespace std;

void E1000Controller::writeCmd(uint16_t addr, uint32_t value) {
    if(barType == PCI_BAR_TYPE_IO) {
        IoWrite32(ioBase, addr);
        IoWrite32(ioBase + 4, value);
    } else {
        MMIOWrite32((uint64_t) memoryBase + addr, value);
    }
}

uint32_t E1000Controller::readCmd(uint16_t addr) {
    if(barType == PCI_BAR_TYPE_IO) {
        IoWrite32(ioBase, addr);
        return IoRead32(ioBase + 4);
    } else {
        return MMIORead32((uint64_t) memoryBase + addr);
    }
}

void E1000Controller::DetectEEProm() {
    uint32_t value = 0;

    writeCmd(REG_EEPROM, 0x1);

    for(int i = 0; i < 1000 && !eePromExists; i++) {
        value = readCmd(REG_EEPROM);
        
        if(value & 0x10)
            eePromExists = true;
        else
            eePromExists = false;
    }
}

uint32_t E1000Controller::EEPromRead(uint8_t addr) {
    uint32_t tmp = 0;

    if(eePromExists) {
        writeCmd(REG_EEPROM, (1) | (uint32_t) addr << 8);
        while(!((tmp = readCmd(REG_EEPROM)) & (1 << 4)));
    } else {
        writeCmd(REG_EEPROM, (1) | ((uint32_t) addr << 2));
        while(!((tmp = readCmd(REG_EEPROM)) & (1 << 1)));
    }

    return (tmp >> 16) & 0xFFFF;
}

bool E1000Controller::CheckMAC() {
    if(!eePromExists) {
        uint32_t* memoryBaseMAC32 = (uint32_t*) memoryBase + 0x5400;
        
        if(memoryBaseMAC32[0] == 0)
            return false;
    }
    return true;
}

void E1000Controller::InitMAC() {
    if(eePromExists) {
        uint32_t tmp;

        tmp = EEPromRead(0);
        MediaAccCtrl[0] = tmp & 0xFF;
        MediaAccCtrl[1] = tmp >> 8;

        tmp = EEPromRead(1);
        MediaAccCtrl[2] = tmp & 0xFF;
        MediaAccCtrl[3] = tmp >> 8;

        tmp = EEPromRead(2);
        MediaAccCtrl[4] = tmp & 0xFF;
        MediaAccCtrl[5] = tmp >> 8;
    }else{
        if(CheckMAC()) {
            uint8_t* memoryBaseMAC8 = (uint8_t*)memoryBase + 0x5400;
            
            if(memoryBaseMAC8[0] != 0) {
                for(int i = 0; i < 6; i++) {
                    MediaAccCtrl[i] = memoryBaseMAC8[i]; 
                }
            }
        }
    }
}

E1000Controller::E1000Controller(srv_pci_bar_info_t* BarInfo) {
    if(BarInfo->Type == PCI_BAR_TYPE_IO) {
        ioBase = (uint64_t) BarInfo->Address;
    } else {
        memoryBase = MapPhysical(BarInfo->Address, BarInfo->Size);
    }
    barType = BarInfo->Type;

    DetectEEProm();
    InitMAC();

    std::printf("[NET/E1000] EEProm: %d BarType: %d MAC0: %x MAC1: %x MAC2: %x MAC3: %x MAC4: %x MAC5: %x", eePromExists, barType, MediaAccCtrl[0], MediaAccCtrl[1], MediaAccCtrl[2], MediaAccCtrl[3], MediaAccCtrl[4], MediaAccCtrl[5]);
}