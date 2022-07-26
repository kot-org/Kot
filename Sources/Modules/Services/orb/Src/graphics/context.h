#pragma once

#include <ramfs/ramfs.h>
#include <kot/utils/vector.h>

typedef struct {
    uint64_t fb_addr;
    size_t fb_size;
    uint32_t width;
    uint32_t height;
    uint32_t pitch;
    uint32_t bpp;
    uint32_t btpp;
} framebuffer_t;

typedef struct {
    uint32_t x;
    uint32_t y;
} pos_t;

class Context {
private:

    uint32_t x = 0;
    uint32_t y = 0;
    vector_t* poses;
    bool _auto = false;
    framebuffer_t* framebuffer;

public:

    Context(framebuffer_t* framebuffer);

    void putPixel(uint32_t x, uint32_t y, uint32_t colour);
    int8_t pixelExist(uint32_t x, uint32_t y);
    uint32_t getPixel(uint32_t x, uint32_t y);

    void fillRect(uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint32_t colour);
    void fillGradientRect(uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint32_t fromColor, uint32_t toColor);
    void drawRect(uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint32_t colour);
    void drawLine(uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2, uint32_t colour);

    // path function

    void fill(uint32_t x, uint32_t y, uint32_t colour);
    void fill(uint32_t x, uint32_t y, uint32_t colour, uint32_t border);

    void draw(uint32_t colour);

    void abs_pos(uint32_t x, uint32_t y);
    void rel_pos(uint32_t x, uint32_t y);

    void add_pos();
    void end_path();
    void reset();

    void setAuto(bool _auto);

    // framebuffer function

    void swapTo(framebuffer_t* to);
    void swapFrom(framebuffer_t* from);
    void swapTo(Context* to);
    void swapFrom(Context* from);
    void blitTo(framebuffer_t* to, uint32_t x, uint32_t y);
    void blitFrom(framebuffer_t* from, uint32_t x, uint32_t y);
    void blitTo(Context* to, uint32_t x, uint32_t y);
    void blitFrom(Context* from, uint32_t x, uint32_t y);
    
    void clear();
    void clear(uint32_t colour);

    framebuffer_t* getFramebuffer();

};