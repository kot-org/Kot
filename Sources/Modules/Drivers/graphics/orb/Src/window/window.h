#include <kot/uisd.h>

#ifndef __ORB__WINDOW__
#define __ORB__WINDOW__

#include <core/main.h>

#define DEFAUT_BPP 32

class monitorc;

struct Cursor{
    int32_t x;
    int32_t y;
};

class windowc{
    public:
        process_t Target;

        framebuffer_t* Framebuffer;
        graphiceventbuffer_t* Eventbuffer;

        ksmem_t FramebufferKey;
        uint64_t XPosition;
        uint64_t YPosition;
        uint64_t WindowType;
        uint64_t WindowIndex;
        bool IsVisible = false;
        bool IsFocus = false;
        event_t Event;

        windowc* Next;
        windowc* Last;
        
        KResult CreateBuffer();

        windowc(uint64_t WindowType, event_t Event);
        monitorc* FindMonitor();
        framebuffer_t* GetFramebuffer();
        graphiceventbuffer_t* GetEventbuffer();
        ksmem_t GetFramebufferKey();
        uint64_t GetHeight();
        uint64_t GetWidth();
        uint64_t GetBpp();
        uint64_t GetPitch();
        uint64_t GetX();
        uint64_t GetY();

        KResult Resize(int64_t Width, int64_t Height);
        KResult Move(int64_t XPosition, int64_t YPosition);

        bool SetFocusState(bool IsFocus);
        bool GetState();

        bool SetVisible(bool IsVisible);
        bool GetVisible();

        KResult Close();
};

extern windowc* CurrentFocusWindow;

#endif