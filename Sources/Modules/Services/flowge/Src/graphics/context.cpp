#include <graphics/context.h>

Context::Context(framebuffer_t* framebuffer) {
    this->framebuffer = framebuffer;
}

void Context::putPixel(uint32_t x, uint32_t y, uint32_t colour) {
    uint8_t* fb = (uint8_t*) this->framebuffer->fb_addr;
    uint64_t index = x * this->framebuffer->btpp + y * this->framebuffer->bps;
    fb[index + 2] = (colour >> 16) & 255;
    fb[index + 1] = (colour >> 8) & 255;
    fb[index] = colour & 255; 
}

void Context::fillRect(uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint32_t colour) {

    uint8_t* fb = (uint8_t*) this->framebuffer->fb_addr;

    for (uint32_t h = y; h < height+y; h++) {
        uint64_t ypos = h * this->framebuffer->bps;
        for (uint32_t w = x; w < width+x; w++) {
            uint64_t xpos = w * this->framebuffer->btpp;
            uint64_t index = ypos + xpos;
            fb[index + 2] = (colour >> 16) & 255;
            fb[index + 1] = (colour >> 8) & 255;
            fb[index] = colour & 255; 
        }
    }

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

    uint8_t* fb2 = (uint8_t*) s2->fb_addr;
    uint8_t* fb1 = (uint8_t*) s1->fb_addr;

    for (size_t h = 0; h < s2->height; h++) {
        size_t ypos_1 = (h + y) * s1->bps;
        size_t ypos_2 = h * s2->bps;
        for (size_t w = 0; w < s2->width; w++) {
            size_t xpos = (w + x) * s1->btpp;
            size_t _i1 = ypos_1+xpos;
            size_t _i2 = ypos_2 + w * s2->btpp;
            fb1[_i1 + 2] = fb2[_i2 + 2];
            fb1[_i1 + 1] = fb2[_i2 + 1];
            fb1[_i1] = fb2[_i2]; 
        }
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