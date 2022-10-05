#pragma once
#include <kot/elf.h>
#include <lib/types.h>
#include <lib/math.h>
#include <bootloader.h>

KResult loadElf(uintptr_t buffer, uint64_t* entrypoint, ukl_kernel_address_t* kernelinfo);