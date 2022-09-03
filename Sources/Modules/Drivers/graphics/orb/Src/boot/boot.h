#pragma once

#include <kot++/printf.h>
#include <kot/uisd/srvs/system.h>

#include "../core/main.h"

struct BGRTHeader_t{
    srv_system_sdtheader_t Header;
    uint16_t VendorID;
    uint8_t Status;
    uint8_t ImageType;
    uint64_t ImageAddress;
    uint32_t ImageOffsetX;
    uint32_t ImageOffsetY;
}__attribute__((packed));

void loadBootGraphics();