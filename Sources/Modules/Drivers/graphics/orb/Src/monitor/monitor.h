#include <kot/heap.h>
#include <kot/utils/vector.h>

#ifndef __ORB__MONITOR__
#define __ORB__MONITOR__

#include <core/main.h>

class windowc;
class Context;

class monitorc {
    private:
        framebuffer_t* MainFramebuffer;
        framebuffer_t* BackFramebuffer;
    public:
        uint64_t XPosition;
        uint64_t XMaxPosition;

        uint64_t YPosition;
        uint64_t YMaxPosition;

        uint64_t XPositionWithDock;
        uint64_t XMaxPositionWithDock;

        uint64_t YPositionWithDock;
        uint64_t YMaxPositionWithDock;

        windowc* DockTop;
        windowc* DockBottom;
        windowc* DockLeft;
        windowc* DockRight;
        
        monitorc(process_t orb, uintptr_t fb_addr, uint64_t Width, uint64_t Height, uint64_t Pitch, uint64_t Bpp, uint32_t XPosition, uint32_t YPosition);
        uint64_t GetWidth();
        uint64_t GetHeight();

        void Move(uint64_t XPosition, uint64_t YPosition);
        void Update(vector_t* Background, vector_t* Windows, vector_t* Foreground);
};

#endif