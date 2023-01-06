#include <kot/uisd.h>

#ifndef __ORB__WINDOW__
#define __ORB__WINDOW__

#include "monitor.h"

class Monitor;

enum FocusState : uint8_t {
    INACTIVE = 0,
    PASSIVE = 1,
    ACTIVE = 2,
};

class Window {
private:
    framebuffer_t* fb;
    ksmem_t fb_key;
    uint32_t xPos;
    uint32_t yPos;
    bool Sshow = false;
    process_t orb;
    process_t owner;
    FocusState focus_state;
    void newBuffer();
public:
    Window(process_t orb, uint64_t width, uint64_t height, uint64_t bpp, uint32_t xPos, uint32_t yPos);
    framebuffer_t* getFramebuffer();
    ksmem_t getFramebufferKey();
    uint32_t getHeight();
    uint32_t getWidth();
    uint32_t getBpp();
    uint32_t getPitch();
    uint32_t getX();
    uint32_t getY();
    void show(bool val);
    void border(bool val);
    void resize(uint32_t width, uint32_t height);
    void move(uint32_t xPos, uint32_t yPos);
    process_t getOwner();
    FocusState getFocusState();
    void setFocusState(FocusState focus_state);
    void destroy();
    bool hasBorder();
    bool isVisible();
};

#endif