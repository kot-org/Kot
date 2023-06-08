#pragma once
#include <lib/elf.h>
#include <lib/types.h>
#include <lib/math.h>
#include <bootloader.h>

KResult loadElf(void* buffer, uint64_t* entrypoint, ukl_kernel_address_t* kernelinfo);