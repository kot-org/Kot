#pragma once
#include "../../io/ports.h"
#include "stdint.h"
#include "../../libc/stdio.h"

struct PCIDevice {
    uint16_t VendorID;
    uint16_t DeviceID;
    uint16_t Function;
    char* Class;
    uint16_t Bus;
    uint16_t Slot;
};

class PCI
{
private:
    uint16_t pciConfigReadWord (uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset);
    uint16_t getVendorID(uint16_t bus, uint16_t device, uint16_t function);
    uint16_t getDeviceID(uint16_t bus, uint16_t device, uint16_t function);
    uint16_t getClassId(uint16_t bus, uint16_t device, uint16_t function);
    uint16_t getSubClassId(uint16_t bus, uint16_t device, uint16_t function);
public:
    PCIDevice Devices[0xFFFF];
    uint16_t DeviceCount;
    void detectDevices();
};
