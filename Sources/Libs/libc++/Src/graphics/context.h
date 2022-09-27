#pragma once

#include <kot/math.h>
#include <kot/utils/map.h>
#include <kot/cstring.h>

namespace std {

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
        bool _scaling = false;

        uint16_t scale;

        uintptr_t fb_addr;
        size64_t fb_size;
        uint32_t width;
        uint32_t height;
        uint32_t pitch;
        uint8_t bpp = 32;
        uint8_t btpp = bpp / 8;

        void subSeqCircle(uint32_t xc, uint32_t yc, uint32_t x, uint32_t y, uint32_t colour);

    public:

        Context(uintptr_t fb_addr, uint32_t width, uint32_t height);

        void putPixel(uint32_t x, uint32_t y, uint32_t colour);
        int8_t pixelExist(uint32_t x, uint32_t y);
        uint32_t getPixel(uint32_t x, uint32_t y);

        void fillRect(uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint32_t colour);
        void drawRect(uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint32_t colour);
        void drawLine(uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2, uint32_t colour);

        void drawCircle(uint32_t xc, uint32_t yc, uint32_t radius, uint32_t colour);

        void fill(uint32_t x, uint32_t y, uint32_t colour);
        void fill(uint32_t x, uint32_t y, uint32_t colour, uint32_t border);

        // path function

        void draw(uint32_t colour);

        void abs_pos(uint32_t x, uint32_t y);
        void rel_pos(uint32_t x, uint32_t y);

        void add_pos();
        void end_path();
        void reset();

        pos_t* get_pos(uint16_t index);

        void auto_pos(bool _auto);
        void scale_pos(bool _scaling);

        uint16_t get_scale();

        // framebuffer function

        void swapTo(uintptr_t to);
        void swapFrom(uintptr_t from);
        void swapTo(Context* to);
        void swapFrom(Context* from);
        
        void clear();
        void clear(uint32_t colour);

        uintptr_t getFramebuffer();

        uint32_t getPitch() {
            return this->pitch;
        }

    };

}