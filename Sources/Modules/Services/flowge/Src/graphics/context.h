#pragma once

#include <kot/heap.h>
#include <kot/math.h>

typedef struct {
    uint64_t fb_addr;
    size_t fb_size;
    uint32_t width;
    uint32_t height;
    uint32_t pitch;
    uint32_t bpp;
    uint32_t bps;
    uint32_t btpp;
} framebuffer_t;

class Context {
private:
    framebuffer_t* framebuffer;
public:
    Context(framebuffer_t* framebuffer);
    void putPixel(uint32_t x, uint32_t y, uint32_t colour);
    void swapTo(framebuffer_t* to);
    void swapFrom(framebuffer_t* from);
    void swapTo(Context* to);
    void swapFrom(Context* from);
    void blitTo(framebuffer_t* to, uint32_t x, uint32_t y);
    void blitFrom(framebuffer_t* from, uint32_t x, uint32_t y);
    void blitTo(Context* to, uint32_t x, uint32_t y);
    void blitFrom(Context* from, uint32_t x, uint32_t y);
    void clear();
    void clearWith(uint8_t value);
    framebuffer_t* getFramebuffer();
};