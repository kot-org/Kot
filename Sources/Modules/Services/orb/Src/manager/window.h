#include <kot/uisd.h>

#ifndef __ORB__WINDOW__
#define __ORB__WINDOW__

#include "monitor.h"

class Monitor;

class Window {
private:
    uintptr_t fb_addr;
    ksmem_t fb_key;
    size64_t fb_size;
    uint32_t width;
    uint32_t height;
    uint32_t pitch;
    int32_t xPos;
    int32_t yPos;
    uint8_t bpp = 32;
    uint8_t btpp = 4; // bpp / 8
    bool Sshow = false;
    bool Sborder = true;
    process_t owner;
public:
    Window(process_t orb, uint32_t width, uint32_t height, int32_t xPos, int32_t yPos);
    uintptr_t getFramebuffer();
    ksmem_t getFramebufferKey();
    uint32_t getHeight();
    uint32_t getWidth();
    uint32_t getPitch();
    int32_t getX();
    int32_t getY();
    void show(bool val);
    void border(bool val);
    void resize(uint32_t width, uint32_t height);
    void move(int32_t xPos, int32_t yPos);
    process_t getOwner();
    void destroy();
};

#endif