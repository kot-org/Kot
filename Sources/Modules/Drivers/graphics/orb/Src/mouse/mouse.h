#pragma once

#include <core/main.h>

#include <kot/uisd/srvs/storage.h>

class orbc;
class windowc;
class monitorc;
class desktopc;
class renderc;
class mousec;


typedef struct {
    uint8_t Revision;
    uint8_t Width;
    uint8_t Height;
    uint64_t PixelMapOffset;
    uint64_t BitmapMaskOffset;
} __attribute__((__packed__)) KursorHeader;


class mousec{
    public:
        mousec(orbc* Parent);
        void CursorInterrupt(int64_t x, int64_t y, int64_t z, uint64_t status);
        void DrawCursor(framebuffer_t* fb);

        orbc* Orb;
        point_t CursorPosition;
        point_t CursorMaxPosition;

        uint64_t CursorWidth;
        uint64_t CursorHeight;

        int64_t Width;
        int64_t Height;

        uintptr_t PixelMap;
        uintptr_t BitmapMask;

        thread_t MouseRelativeInterrupt;

        bool IsLastLeftClick = false;
};