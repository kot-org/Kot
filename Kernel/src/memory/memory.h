#pragma once

#include "efiMemory.h"

uint64_t GetMemorySize(EFI_MEMORY_DESCRIPTOR* mMap, uint64_t nMApEntries, uint64_t mMapDescSize);
void memset(void* start, uint8_t value, uint64_t num);
int memcmp(const void *aptr, const void *bptr, size_t n);

void memcpy(void* destination, void* source, uint64_t num);