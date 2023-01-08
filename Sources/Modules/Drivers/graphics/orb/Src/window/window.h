#include <kot/uisd.h>

#ifndef __ORB__WINDOW__
#define __ORB__WINDOW__

#include <core/main.h>

#define DEFAUT_Bpp 32

class Monitor;

struct Cursor{
    int32_t x;
    int32_t y;
};

class Window {
    private:
        framebuffer_t* Framebuffer;
        ksmem_t FramebufferKey;
        uint64_t XPositionition;
        uint64_t YPositionition;
        uint64_t WindowType;
        uint64_t WindowIndex;
        bool IsVisible = false;
        bool IsFocus = false;
        
        KResult CreateBuffer();
    public:
        Window(uint64_t WindowType);
        framebuffer_t* GetFramebuffer();
        ksmem_t GetFramebufferKey();
        uint64_t GetHeight();
        uint64_t GetWidth();
        uint64_t GetBpp();
        uint64_t GetPitch();
        uint64_t GetX();
        uint64_t GetY();

        KResult Resize(int64_t Width, int64_t Height);
        KResult Move(int64_t XPositionition, int64_t YPositionition);

        void SetState(bool IsFocus);
        bool GetState();

        void SetVisible(bool IsVisible);
        bool GetVisible();

        void Close();
};

#endif