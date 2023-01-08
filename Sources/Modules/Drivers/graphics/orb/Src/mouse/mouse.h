#pragma once

#include <core/main.h>

extern Point_t CursorPosition;
extern Point_t CursorMaxPosition;

typedef struct {
    uint8_t Revision;
    uint8_t Width;
    uint8_t Height;
    uint64_t PixelMapOffset;
    uint64_t BitmapMaskOffset;
} __attribute__((__packed__)) KursorHeader;

extern uintptr_t PixelMap;
extern uintptr_t BitmapMask;

void InitializeCursor();
void CursorInterrupt(int64_t x, int64_t y, int64_t z, uint64_t status);

void DrawCursor(framebuffer_t* fb, uintptr_t BitmapMask, uintptr_t PixelMap);