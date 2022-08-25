#pragma once

#define PCI_CONFIG_ADDR     0xCF8
#define PCI_CONFIG_DATA     0xCFC

#include <kot/sys.h>

void PCIWrite32(uint32_t addr, uint32_t data);

uint32_t PCIRead32(uint32_t addr);
uint16_t PCIRead16(uint32_t addr);
uint8_t PCIRead8(uint32_t addr);