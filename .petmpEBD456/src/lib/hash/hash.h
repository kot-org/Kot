#pragma once

#include <stdint.h>

namespace hash{
    uint32_t CRC32(uint8_t* data, uint32_t size);
}