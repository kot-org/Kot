
#pragma once

#include "memory/Memory.h"
#include "Bitmap.h"
#include "paging/PageFrameAllocator.h"
#include "paging/PageMapIndexer.h"
#include "paging/Paging.h"
#include "paging/PageTableManager.h"
#include "graphics/BasicRenderer.h"
#include "graphics/SimpleFont.h"
#include "uefi/FrameBuffer.h"
#include "uefi/EfiMemory.h"
#include "acpi/rsdp.h"
#include "../../bios/multiboot.h"

#define MIN(arg1,arg2) ((arg1) > (arg2) ? (arg2) : (arg1))
#define MAX(arg1,arg2) ((arg1) > (arg2) ? (arg1) : (arg2))

struct BootInfo {
	Framebuffer* framebuffer;
	PSF1_FONT* psf1_font;
	EFI_MEMORY_DESCRIPTOR* mMap;
	uint64_t mMapSize;
	uint64_t mMapDescriptorSize;
	rsdp_t* rsdp;
};

extern uint64_t _KernelStart;
extern uint64_t _KernelEnd;

struct KernelInfo {
    PageTableManager* pageTableManager;
};

KernelInfo InitializeKernel(BootInfo* bootInfo);
KernelInfo InitializeKernel(multiboot_info_t* bootInfo);
PageTableManager* KernelPageTableManager();
const void* SystemRSDPAddress();
void InitializeNewPageTableManager(PageTableManager& ptm);