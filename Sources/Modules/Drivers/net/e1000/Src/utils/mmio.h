#pragma once

#include <kot/sys.h>

void MMIOWrite32(uintptr_t addr, uint64_t reg, uint32_t value);

uint8_t MMIORead8(uintptr_t addr, uint64_t reg);
uint32_t MMIORead32(uintptr_t addr, uint64_t reg);