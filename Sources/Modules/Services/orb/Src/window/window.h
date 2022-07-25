#pragma once

#include <kot/heap.h>
#include <graphics/context.h>

class Window {
private:
    Context* context = NULL;
    uint32_t width;
    uint32_t height;
    uint32_t x;
    uint32_t y;
    bool _show = false;
    bool _showBorders = true;
    char* title = NULL;
public:
    Window(Context* from, uint32_t x, uint32_t y, uint32_t width, uint32_t height);
    void show();
    void hide();
    void showBorders();
    void hideBorders();
    void render(Context* to);
    void move(uint32_t x, uint32_t y);
    void resize(uint32_t width, uint32_t height);
    void setTitle(char* title);
    char* getTitle();
    uint32_t getHeight();
    uint32_t getWidth();
    uint32_t getX();
    uint32_t getY();
    Context* getContext();
};