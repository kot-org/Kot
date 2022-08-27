#include <kot/heap.h>
#include <kot/utils/vector.h>
#include <kot++/graphics/context.h>

#ifndef __ORB__MONITOR__
#define __ORB__MONITOR__

#include "window.h"

class Window;
class Context;

class Monitor {
private:
    uintptr_t fb_addr;
    uintptr_t bb_addr;
    size64_t fb_size;
    uint32_t width;
    uint32_t height;
    uint32_t pitch;
    int32_t xPos;
    int32_t yPos;
    uint8_t bpp = 32;
    uint8_t btpp = 4; // bpp / 8
    Window* background;
public:
    Monitor(uintptr_t framebuffer, uint32_t width, uint32_t height, int32_t xPos, int32_t yPos);
    uint32_t getWidth();
    uint32_t getHeight();
    Window* getBackground();
    void setBackground(Window* w);
    void move(int32_t xPos, int32_t yPos);
    void update(vector_t* windows);
};

#endif