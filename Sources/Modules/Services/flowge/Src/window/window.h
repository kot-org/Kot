#pragma once

#include <kot/heap.h>
#include <graphics/context.h>

class Window
{
private:
    Context context;
    uint32_t width;
    uint32_t height;
    uint32_t x;
    uint32_t y;
    bool _show = false;
    char *title = NULL;
    WindowStyle style;

public:
    Window(Context *from, uint32_t x, uint32_t y, uint32_t width, uint32_t height);
    Window(Context *from, uint32_t x, uint32_t y, uint32_t width, uint32_t height, WindowStyle style);
    void show();
    void hide();
    void render(Context *to);
    void move(uint32_t x, uint32_t y);
    void resize(uint32_t width, uint32_t height);
    void setTitle(char *title);
    char *getTitle();
    uint32_t getHeight();
    uint32_t getWidth();
    Context *getContext();
    WindowStyle getStyle();
};