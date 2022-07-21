#pragma once

#include <core/main.h>

// class WindowContext {
// private:
// public:
//     WindowContext();
// };

class Window {
private:
    screen_t* framebuffer;
    uint32_t width;
    uint32_t height;
    uint32_t x;
    uint32_t y;
    bool _show = false;
public:
    Window(screen_t* screen);
    Window(screen_t* screen, uint32_t x, uint32_t y);
    Window(screen_t* screen, uint32_t x, uint32_t y, uint32_t width, uint32_t height);
    void show();
    void hide();
    screen_t* getFramebuffer();
};