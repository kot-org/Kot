#pragma once

#include <kot/heap.h>
#include <core/main.h>
#include <kot/utils/vector.h>

class orbc;
class windowc;
class monitorc;
class desktopc;
class renderc;
class hidc;


class monitorc {
    public:
        framebuffer_t* MainFramebuffer;
        framebuffer_t* BackFramebuffer;

        graphiceventbuffer_t* Eventbuffer;

        uint64_t XPosition;
        uint64_t XMaxPosition;

        uint64_t YPosition;
        uint64_t YMaxPosition;

        uint64_t XPositionWithDock;
        uint64_t XMaxPositionWithDock;

        uint64_t YPositionWithDock;
        uint64_t YMaxPositionWithDock;

        struct desktopmonitor* DesktopData;

        orbc* Orb;
        
        monitorc(orbc* Parent, uintptr_t FbBase, uint64_t Width, uint64_t Height, uint64_t Pitch, uint64_t Bpp, uint32_t XPosition, uint32_t YPosition);
        uint64_t GetWidth();
        uint64_t GetHeight();

        void Move(uint64_t XPosition, uint64_t YPosition);

        void SetWindowEvent(windowc* window);
        void UpdateEvents(windowc* FirstWindowNode);

        void Update(windowc* FirstWindowNode);
};