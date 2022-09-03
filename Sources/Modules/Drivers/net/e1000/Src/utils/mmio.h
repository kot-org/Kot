#pragma once

#include <kot/sys.h>

void MMIOWrite32(uint64_t addr, uint32_t value);
uint32_t MMIORead32(uint64_t addr);