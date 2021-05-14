#pragma once

#include "../efiMemory.h"
#include "../hardware/acpi/acpi.h"

/* font */
typedef struct {
    uint8_t        Signature[2];
    uint8_t        mode;
    uint8_t        charsize;
    char*        glyphBuffer[256];
} PSF_FONT __attribute__((packed));

/* GOP */
typedef struct {
	void* BaseAddress;
	void* BaseAddressBackground;
    size_t FrameBufferSize;
	unsigned int Width;
	unsigned int Height;
	unsigned int PixelsPerScanLine;
} Framebuffer __attribute__((packed));

struct BootInfo {
	void* KernelStart;
	size_t KernelSize;
	Framebuffer* framebuffer;
	PSF_FONT* psf1_Font;	
	EFI_MEMORY_DESCRIPTOR* mMap;
	uint64_t mMapSize;
	uint64_t mMapDescSize;
	ACPI::RSDP2* rsdp;
} __attribute__((packed)); 