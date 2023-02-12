#pragma once

#include <core/main.h>

struct BGRTHeader_t{
    srv_system_sdtheader_t Header;
    uint16_t VendorID;
    uint8_t Status;
    uint8_t PictureboxType;
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

void LoadBootGraphics(framebuffer_t* Framebuffer);

void ParseBootImage(framebuffer_t* Framebuffer, uint8_t* IGA, uint32_t Width, uint32_t Height, uint8_t Bpp, uint32_t XPosition, uint32_t YPosition);

void LoadBootAnimation(framebuffer_t* Framebuffer, uint64_t XPosition, uint64_t YPosition, uint64_t Width, uint64_t Height);
void BootAnimation(framebuffer_t* Framebuffer, uint64_t XPosition, uint64_t YPosition, uint64_t Width, uint64_t Height);