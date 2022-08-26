#include <tools/config.h>

void PCIWrite32(uint32_t addr, uint32_t data) {
    addr &= ~(0b11);
    /* Write address */
    IoWrite32(PCI_CONFIG_ADDR, addr);
    /* Write data */
    IoWrite32(PCI_CONFIG_DATA, data);
}

uint32_t PCIRead32(uint32_t addr) {
    addr &= ~(0b11);
    /* Write address */
    IoWrite32(PCI_CONFIG_ADDR, addr);
    /* Read data */
    return IoRead32(PCI_CONFIG_DATA);
}

uint16_t PCIRead16(uint32_t addr) {
    uint8_t offset = addr & 0xff;
    addr &= ~(0b11);
    return (uint16_t) ((PCIRead32(addr) >> ((offset & 0b10) * 0x10)) & 0xffff);
}

uint8_t PCIRead8(uint32_t addr) {
    uint8_t offset = addr & 0xff;
    addr &= ~(0b11);
    return (uint16_t) ((PCIRead16(addr) >> ((offset & 0b1) * 0x8)) & 0xff);
}