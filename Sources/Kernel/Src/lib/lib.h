#pragma once
#include <kot/types.h>

bool ReadBit(uint8_t byte, int position);
uint8_t WriteBit(uint8_t byte, int position, bool value);
uint64_t DivideRoundUp(uint64_t value, uint64_t divider);