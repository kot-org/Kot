#pragma once
#include <stdint.h>
#include "pcidefs.h"

class PCITranslate
{
public:

    const char* TranslateClass(uint8_t classid);
    const char* TranslateSubClass(uint8_t classid,uint8_t subclassid);
    const char* TranslateVendor(uint16_t vendorid);
    const char* TranslateDeviceID(uint16_t vendorid,uint16_t deviceid);
    TranslatedPCIDevice TranslateDevice(struct PCIDevice* device);
};
