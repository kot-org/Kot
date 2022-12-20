#pragma once

#include <kot++/printf.h>
#include <kot/uisd/srvs/system.h>
#include <kot-graphics++/utils.h>

#include "../core/main.h"

using namespace Graphic;

struct BGRTHeader_t{
    srv_system_sdtheader_t Header;
    uint16_t VendorID;
    uint8_t Status;
    uint8_t ImageType;
    uint64_t ImageAddress;
    uint32_t ImageOffsetX;
    uint32_t ImageOffsetY;
}__attribute__((packed));

struct BMPImageHeader_t{
    uint16_t MagicNumber;
    uint32_t Size;
    uint32_t Reserved;
    uint32_t ImageOffset;
    uint32_t HeaderSize;
    uint32_t Width;
    uint32_t Height;
    uint16_t Planes;
    uint16_t Bpp;
    uint32_t CompressionType;
    uint32_t ImageSize;
    uint32_t XPixelsPerMeter;
    uint32_t YPixelsPerMeter;
    uint32_t NumberOfColors;
    uint32_t ImportantColors;
}__attribute__((packed));

void loadBootGraphics(framebuffer_t* Framebuffer);

void parseBootImage(framebuffer_t* Framebuffer, uint8_t* IGA, uint32_t Width, uint32_t Height, uint8_t Bpp, uint32_t XPos, uint32_t YPos);

void loadBootAnimation(framebuffer_t* Framebuffer, uint64_t XPos, uint64_t YPos, uint64_t Width, uint64_t Height);
void bootAnimation(framebuffer_t* Framebuffer, uint64_t XPos, uint64_t YPos, uint64_t Width, uint64_t Height);