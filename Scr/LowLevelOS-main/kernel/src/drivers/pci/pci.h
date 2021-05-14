#pragma once
#include <stdint.h>
#include "../../misc/power/acpi.h"
#include "pcitranslate.h"

class PCI {
private:
    void EnumFunc(uint64_t addr,uint64_t function);
    void EnumDevice(uint64_t addr, uint64_t device);
    void EnumBus(uint64_t addr, uint64_t bus);
public:
    TranslatedPCIDevice Devices[0x2000];
    int DevicesIndex = 0;
    void EnumeratePCI(MCFG* mcfg);
};