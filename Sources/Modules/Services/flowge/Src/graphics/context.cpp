#include <graphics/context.h>

Context::Context(framebuffer_t* framebuffer) {
    this->framebuffer = framebuffer;
}

void Context::putPixel(uint32_t x, uint32_t y, uint32_t colour) {
    uint8_t* fb = (uint8_t*) this->framebuffer->fb_addr;
    uint64_t index = x * this->framebuffer->btpp + y * this->framebuffer->pitch;
    fb[index + 2] = (colour >> 16) & 255;
    fb[index + 1] = (colour >> 8) & 255;
    fb[index] = colour & 255; 
}

void Context::fillRect(uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint32_t colour) {

    uint8_t* fb = (uint8_t*) this->framebuffer->fb_addr;

    for (uint32_t h = y; h < height+y; h++) {
        uint64_t ypos = h * this->framebuffer->pitch;
        for (uint32_t w = x; w < width+x; w++) {
            uint64_t xpos = w * this->framebuffer->btpp;
            uint64_t index = ypos + xpos;
            fb[index + 2] = (colour >> 16) & 255;
            fb[index + 1] = (colour >> 8) & 255;
            fb[index] = colour & 255; 
        }
    }

}

void Context::drawLine(uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2, uint32_t colour) {

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

    this->putPixel(x, y, colour);

    for (int32_t i = 0; i < abs(dx); i++) {
        if (p < 0) {
            if (isSwaped == 0) {
                x = x + sx;
                this->putPixel(x, y, colour);
            } else {
                y = y+sy;
                this->putPixel(x, y, colour);
            }
            p = p + 2*abs(dy);
        } else {
            x = x+sx;
            y = y+sy;
            this->putPixel(x, y, colour);
            p = p + 2*abs(dy) - 2*abs(dx);
        }
    }

}

void Context::fillTri(uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2, uint32_t x3, uint32_t y3, uint32_t colour) {
    // todo    
}

void Context::drawTri(uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2, uint32_t x3, uint32_t y3, uint32_t colour) {
    this->drawLine(x1, y1, x2, y2, colour);
    this->drawLine(x2, y2, x3, y3, colour);
    this->drawLine(x1, y1, x3, y3, colour);
}

void Context::drawRect(uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint32_t colour) {
    this->drawLine(x, y, x+width, y, colour); // top
    this->drawLine(x, y+height, x+width, y+height, colour); // bottom
    this->drawLine(x, y, x, y+height, colour); // left
    this->drawLine(x+width, y, x+width, y+height, colour); // right
}

void Context::swapTo(framebuffer_t* to) {
    memcpy((uintptr_t)to->fb_addr, (uintptr_t)this->framebuffer->fb_addr, this->framebuffer->fb_size);
}

void Context::swapFrom(framebuffer_t* from) {
    memcpy((uintptr_t)this->framebuffer->fb_addr, (uintptr_t)from->fb_addr, from->fb_size);
}

void Context::swapTo(Context* to) {
    this->swapTo(to->getFramebuffer());
}

void Context::swapFrom(Context* from) {
    this->swapFrom(from->getFramebuffer());
}

void blitFramebuffer(framebuffer_t* s1, framebuffer_t* s2, uint32_t x, uint32_t y) {

    uint64_t to = s1->fb_addr;
    uint64_t from = s2->fb_addr;

    to += x * s1->btpp + y * s1->pitch; // offset

    for (uint64_t h = 0; h < s2->height; h++) {
        memcpy((uintptr_t) to, (uintptr_t) from, (uint64_t) s2->pitch);
        to += s1->pitch;
        from += s2->pitch;
    }

}

void Context::blitTo(framebuffer_t* to, uint32_t x, uint32_t y) {
    blitFramebuffer(to, this->getFramebuffer(), x, y);
}

void Context::blitFrom(framebuffer_t* from, uint32_t x, uint32_t y) {
    blitFramebuffer(this->getFramebuffer(), from, x, y);
}

void Context::blitTo(Context* to, uint32_t x, uint32_t y) {
    this->blitTo(to->getFramebuffer(), x, y);
}

void Context::blitFrom(Context* from, uint32_t x, uint32_t y) {
    this->blitFrom(from->getFramebuffer(), x, y);
}

void Context::fill(uint32_t colour) {
    memset32((uintptr_t) this->framebuffer->fb_addr, colour, this->framebuffer->fb_size);
} 

void Context::clear() {
    memset((uintptr_t) this->framebuffer->fb_addr, 0x00, this->framebuffer->fb_size);
} 

framebuffer_t* Context::getFramebuffer() {
    return this->framebuffer;
}