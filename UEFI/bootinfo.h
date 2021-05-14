#ifndef BOOTINFO_H
#define BOOTINFO_H

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

/* Boot info stuct */

typedef struct {
    /* debug offset */
	void* KernelStart;
	size_t KernelSize;
	Framebuffer* framebuffer;
	PSF_FONT* psf1_Font;
	EFI_MEMORY_DESCRIPTOR* mMap;
	uint64_t mMapSize;
	uint64_t mMapDescSize;
    void* rsdp; //read system descriptor pointer
} BootInfo __attribute__((packed));

#endif