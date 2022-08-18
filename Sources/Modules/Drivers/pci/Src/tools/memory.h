#pragma once

#include <kot/sys.h>

void PCIMemcpyToMemory8(uintptr_t dst, uint32_t src, size_t size);
void PCIMemcpyToMemory16(uintptr_t dst, uint32_t src, size_t size);
void PCIMemcpyToMemory32(uintptr_t dst, uint32_t src, size_t size);