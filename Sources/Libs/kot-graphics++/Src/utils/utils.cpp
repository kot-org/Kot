#include <kot-graphics++/utils.h>

namespace Graphic {

    extern "C" void blendAlpha(uintptr_t Pixel, uint32_t color);

    int8_t pixelExist(framebuffer_t* fb, uint32_t x, uint32_t y) {
        if (x < 0 || y < 0) return -1;
        if (x > fb->width || y > fb->height) return -1;
        return 1;
    }

    void putPixel(framebuffer_t* fb, uint32_t x, uint32_t y, uint32_t colour) {
        if (pixelExist(fb, x, y) == -1) return;
        uint64_t index = x * fb->btpp + y * fb->pitch;
        blendAlpha((uintptr_t)((uint64_t)fb->addr + index), colour);
    }

    uint32_t getPixel(framebuffer_t* fb, uint32_t x, uint32_t y) {
        uint64_t index = x * fb->btpp + y * fb->pitch;
        return *(uint32_t*)((uint64_t) fb->addr + index);
    }

    void blitFramebuffer(framebuffer_t* to, framebuffer_t* from, uint32_t x, uint32_t y) {

        uint64_t to_addr = (uint64_t) to->addr;
        uint64_t from_addr = (uint64_t) from->addr;

        to_addr += x * to->btpp + y * to->pitch; // offset

        uint64_t num;

        if (to->pitch < from->pitch) {
            num = to->pitch;
        } else {
            num = from->pitch;
        } 

        for (uint32_t h = 0; h < from->height && h + y < to->height; h++) {
            memcpy((uintptr_t) to_addr, (uintptr_t) from_addr, num);
            to_addr += to->pitch;
            from_addr += from->pitch;
        }
    
    }

    void fillRect(framebuffer_t* fb, uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint32_t colour) {

        uint32_t _h = height+y;
        uint32_t _w = width+x;

        if (_h > fb->height) { _h = fb->height; }
        if (_w > fb->width) { _w = fb->width; }

        for (uint32_t h = y; h < _h; h++) {
            uint64_t ypos = h * fb->pitch;
            for (uint32_t w = x; w < _w; w++) {
                uint64_t xpos = w * fb->btpp;
                uint64_t index = ypos + xpos;
                blendAlpha((uintptr_t)((uint64_t)fb->addr + index), colour);
            }
        }

    }

    void drawLine(framebuffer_t* fb, uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2, uint32_t colour) {

        if (x1 > fb->width) { x1 = fb->width; }
        if (y1 > fb->height) { y1 = fb->height; }
        if (x2 > fb->width) { x2 = fb->width; }
        if (y2 > fb->height) { y2 = fb->height; }

        if (x1 < 0) { x1 = 0; }
        if (y1 < 0) { y1 = 0; }
        if (x2 < 0) { x2 = 0; }
        if (y2 < 0) { y2 = 0; }

        int32_t dx = x2-x1;
        int32_t dy = y2-y1;

        int8_t sx = sgn(dx);
        int8_t sy = sgn(dy);

        int32_t x = x1;
        int32_t y = y1;

        int8_t isSwaped = 0;

        if(abs(dy) > abs(dx)) {
            int32_t tdx = dx;
            dx = dy;
            dy = tdx;
            isSwaped = 1;
        }

        int32_t p = 2*(abs(dy)) - abs(dx);

        putPixel(fb, x, y, colour);

        for (int32_t i = 0; i < abs(dx); i++) {
            if (p < 0) {
                if (isSwaped == 0) {
                    x = x + sx;
                    putPixel(fb, x, y, colour);
                } else {
                    y = y+sy;
                    putPixel(fb, x, y, colour);
                }
                p = p + 2*abs(dy);
            } else {
                x = x+sx;
                y = y+sy;
                putPixel(fb, x, y, colour);
                p = p + 2*abs(dy) - 2*abs(dx);
            }
        }

    }

    void drawRect(framebuffer_t* fb, uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint32_t colour) {
        drawLine(fb, x, y, x+width, y, colour); // top
        drawLine(fb, x, y+height, x+width, y+height, colour); // bottom
        drawLine(fb, x, y, x, y+height, colour); // left
        drawLine(fb, x+width, y, x+width, y+height, colour); // right
    }
    
} 
