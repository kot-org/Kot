#include "e1000.h"
#include "arch/x86_64/io/io.h"

constexpr uint16_t REG_EEPROM = 0x0014;

void E1000::writeCommand(uint16_t address, uint32_t val) const {
    if(_barType == 0) {
        mmio_write_32(_mmioBase + address, val);
    } else {
        port_write_32(_ioBase, address);
        port_write_32(_ioBase + 4, val);
    }
}

uint32_t E1000::readCommand(uint16_t address) const {
    if(_barType == 0) {
        return mmio_read_32(_mmioBase + address);
    }

    port_write_32(_ioBase, address);
    return port_read_32(_ioBase + 4);
}

bool E1000::detectEEPROM() {
    uint32_t val = 0;
    _hasEEPROM = false;
    writeCommand(REG_EEPROM, 0x1);
    for(int i = 0; i < 1000; i++) {
        val = readCommand(REG_EEPROM);
        if(val & 0x10) {
            return (_hasEEPROM = true);
        }
    }

    return false;
}

uint32_t E1000::readEEPROM(uint8_t addr) const {
    uint16_t data = 0;
    uint32_t tmp = 0;
    if(_hasEEPROM) {
        writeCommand(REG_EEPROM, 1 | (((uint32_t)addr) << 8));
        while(!((tmp = readCommand(REG_EEPROM)) & (1 << 4)));
    } else {
        writeCommand(REG_EEPROM, 1 | (((uint32_t)addr) << 2));
        while(!((tmp = readCommand(REG_EEPROM)) & (1 << 1)));
    }

    data = (uint16_t)((tmp >> 16) & 0xFFFF);
    return data;
}