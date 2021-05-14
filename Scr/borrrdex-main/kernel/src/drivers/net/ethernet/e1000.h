#pragma once

#include "pci/pci.h"
#include <stdint.h>

constexpr uint16_t INTEL_VEND = 0x8086;
constexpr uint16_t E1000_DEV = 0x100e;

typedef struct {
    volatile uint64_t addr;
    volatile uint16_t length;
    volatile uint16_t checksum;
    volatile uint8_t status;
    volatile uint8_t errors;
    volatile uint16_t special;
} __attribute__((packed)) e1000_rx_desc_t;

typedef struct {
    volatile uint64_t addr;
    volatile uint16_t length;
    volatile uint8_t cso;
    volatile uint8_t cmd;
    volatile uint8_t status;
    volatile uint8_t css;
    volatile uint16_t special;
} __attribute__((packed)) e1000_tx_desc_t;

class E1000 {
public:
    E1000(pci_header_t* pci_header);

    void start();
    int sendPacket(const void* data, uint16_t len);

private:
    void writeCommand(uint16_t address, uint32_t val) const;
    uint32_t readCommand(uint16_t address) const;

    bool detectEEPROM();
    uint32_t readEEPROM(uint8_t addr) const;
    
    uint8_t _barType;
    uint64_t _mmioBase;
    uint16_t _ioBase;
    bool _hasEEPROM;
};