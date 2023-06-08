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
        orbc* Orb;
        kot_process_t Target;

        kot_framebuffer_t* Framebuffer;
        graphiceventbuffer_t* Eventbuffer;

        kot_key_mem_t FramebufferKey;
        uint64_t XPosition;
        uint64_t YPosition;
        uint64_t WindowType;
        bool IsVisible = false;
        bool IsFocus = false;

        kot_event_t Event;
        struct hid_event_t* MouseEvent;

        windowc* Next;
        windowc* Last;
        
        KResult CreateBuffer();

        windowc(orbc* Parent, uint64_t WindowType, kot_event_t Event);
        monitorc* FindMonitor();
        kot_framebuffer_t* GetFramebuffer();
        graphiceventbuffer_t* GetEventbuffer();
        kot_key_mem_t GetFramebufferKey();
        uint64_t GetHeight();
        uint64_t GetWidth();
        uint64_t GetBpp();
        uint64_t GetPitch();
        uint64_t GetX();
        uint64_t GetY();

        KResult Resize(int64_t Width, int64_t Height);
        KResult Move(int64_t XPosition, int64_t YPosition);

        bool SetFocusState(bool IsFocus);
        bool GetFocusState();

        bool SetVisible(bool IsVisible);
        bool GetVisible();

        KResult Close();

        KResult Enqueu();
        KResult EnqueuWL();
        KResult Dequeu();
        KResult DequeuWL();
};

extern windowc* CurrentFocusWindow;

#endif