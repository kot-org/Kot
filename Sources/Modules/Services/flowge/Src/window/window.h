#pragma once

#include <kot/heap.h>
#include <graphics/context.h>

class Window {
private:
    Context context;
    uint32_t width;
    uint32_t height;
    uint32_t x;
    uint32_t y;
    bool _show = false;
public:
    Window(Context* from, uint32_t x, uint32_t y, uint32_t width, uint32_t height);
    void show();
    void hide();
    void render(Context* to);
    Context* getContext();
};