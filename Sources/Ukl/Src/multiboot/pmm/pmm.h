#pragma once

#include <lib/math.h>
#include <multiboot/memory/memory.h>
#include <multiboot/core/multiboot.h>

#define FIRST_USABLE_MEMORY   0x10000
#define MAX_ENTRY_IN_PAGE_UKL (PAGE_SIZE / sizeof(struct ukl_mmap_info_t))

extern uint32_t Pmm_MbHeader;
extern uint32_t Pmm_SizeMbHeader;
extern struct multiboot_tag_mmap* Pmm_Map;
extern uint64_t Pmm_LastIndexAllocated;
extern uint64_t Pmm_LastAddressAllocated;
extern uint64_t Pmm_PageNumber;
extern uint64_t Pmm_Size;

extern uint32_t __attribute__((section(".start"))) Pmm_Ukl_Start;
extern uint32_t __attribute__((section(".end"))) Pmm_Ukl_End;

void Pmm_Init(uint32_t MbHeader, struct multiboot_tag_mmap* Map);

uint64_t Pmm_GetMemorySize(struct multiboot_tag_mmap* Map);

void Pmm_InitBitmap(size64_t bitmapSize, uint64_t bufferAddress);

uint64_t Pmm_RequestPage();

void Pmm_ExtractsInfo(struct ukl_boot_structure_t* BootData);