#pragma once

#include <lib/types.h>

extern "C" void boot_kernel(uint32_t paging, uint64_t entry_point, uint64_t stack, uint64_t arg0);