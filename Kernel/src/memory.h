#pragma once

#include <stdint.h>
#include "efiMemory.h"

uint64_t GetMemorySize(EFI_MEMORY_DESCRIPTOR* mMap, uint64_t nMApEntries, uint64_t mMapDescSize);
void memset(void* start, uint8_t value, uint64_t num);

void memcpy(void* destination, void* source, uint64_t num);