#include <kot/heap.h>
#include <kot/utils/vector.h>
#include <kot++/graphics/utils.h>
#include <kot++/graphics/context.h>

using namespace std;

#ifndef __ORB__MONITOR__
#define __ORB__MONITOR__

#include "window.h"

class Window;
class Context;

class Monitor {
private:
    framebuffer_t* main;
    framebuffer_t* back;
    uint32_t xPos;
    uint32_t yPos;
    Window* background;
public:
    Monitor(process_t orb, uintptr_t fb_addr, uint64_t width, uint64_t height, uint64_t pitch, uint64_t bpp, uint32_t xPos, uint32_t yPos);
    uint32_t getWidth();
    uint32_t getHeight();
    Window* getBackground();
    void setBackground(Window* w);
    void move(uint32_t xPos, uint32_t yPos);
    void update(vector_t* windows);
};

#endif