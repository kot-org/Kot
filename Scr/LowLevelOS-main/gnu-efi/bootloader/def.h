#pragma once

#include "uefiutils.h"
#include "clib.h"

#define PSF1_MAGIC0 0x36
#define PSF1_MAGIC1 0x04

typedef struct
{
	void *PowerOff;
	void *Restart;
} Power;

typedef struct
{
	void *BaseAddr;
	long long BufferSize;
	unsigned int Width;
	unsigned int Height;
	unsigned int PixelPerScanLine;
} framebuffer;

typedef struct
{
	CHAR16 *Vendor;
	uint32_t Version;
} UEFIFirmware;

typedef struct
{
	unsigned char magic[2];
	unsigned char mode;
	unsigned char charsize;
} PSF1_HEADER;

typedef struct
{
	PSF1_HEADER *psf1_Header;
	void *glyphBuffer;
} PSF1_FONT;

typedef struct
{
	framebuffer *framebuf;
	PSF1_FONT *font;
	Power *pwr;
	UEFIFirmware *firm;
	EFI_MEMORY_DESCRIPTOR *mMap;
	UINTN mMapSize;
	UINTN mMapDescSize;
	uint64_t Key;
	void* RSDP;
} BootInfo;

typedef struct {
   char  identsize;
   char  colourmaptype;
   char  datatypecode;
   short int colourmaporigin;
   short int colourmaplength;
   char  colourmapdepth;
   short int x_origin;
   short int y_origin;
   short width;
   short height;
   char  bitsperpixel;
   char  imagedescriptor;
} TGAHeader;
