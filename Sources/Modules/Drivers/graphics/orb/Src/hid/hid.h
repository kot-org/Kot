#pragma once

#include <core/main.h>

#include <kot/uisd/srvs/storage.h>

class orbc;
class windowc;
class monitorc;
class desktopc;
class renderc;
class hidc;


typedef struct {
    uint8_t Revision;
    uint8_t Width;
    uint8_t Height;
    uint64_t PixelMapOffset;
    uint64_t BitmapMaskOffset;
} __attribute__((__packed__)) KursorHeader;

#define MOUSE_EVENT_PARENT_TYPE_WINDOW  0x0
#define MOUSE_EVENT_PARENT_TYPE_WIDGET  0x1

struct hid_event_t{
    event_t Event;
    uint64_t ParentType;
    uintptr_t Parent;
};


class hidc{
    public:
        hidc(orbc* Parent);
        void KeyboardInterrupt(uint64_t KeyCode);
        void CursorInterrupt(int64_t x, int64_t y, int64_t z, uint64_t status);
        void DrawCursor(framebuffer_t* fb);

        hid_event_t* CurrentFocusEvent;

        orbc* Orb;

        thread_t KeyboardInterruptThread;

        point_t CursorPosition;
        point_t CursorMaxPosition;

        uint64_t CursorWidth;
        uint64_t CursorHeight;

        int64_t Width;
        int64_t Height;

        uintptr_t PixelMap;
        uintptr_t BitmapMask;

        thread_t MouseRelativeInterruptThread;

        bool IsLastLeftClick = false;
};