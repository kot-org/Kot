#include "monitor.h"

#include <kot++/printf.h>

uint8_t MouseMask[] = {
    0b10000000, 0b00000000, 
    0b11000000, 0b00000000, 
    0b11100000, 0b00000000, 
    0b11110000, 0b00000000, 
    0b11111000, 0b00000000, 
    0b11111100, 0b00000000, 
    0b11111110, 0b00000000, 
    0b11111111, 0b00000000, 
    0b11111111, 0b10000000, 
    0b11111111, 0b11000000, 
    0b11111111, 0b11100000, 
    0b11111110, 0b00000000, 
    0b11101111, 0b00000000, 
    0b11001111, 0b00000000, 
    0b10000111, 0b10000000, 
    0b00000111, 0b10000000, 
    0b00000011, 0b11000000, 
    0b00000011, 0b11000000, 
    0b00000001, 0b10000000, 
};

uint32_t CursorColor[CursorHeight][CursorWidth] = 
{
    { 0x2B2B2B, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF },
    { 0x2B2B2B, 0x2B2B2B, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF },
    { 0x2B2B2B, 0xFFFFFF, 0x2B2B2B, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF },
    { 0x2B2B2B, 0xFFFFFF, 0xFFFFFF, 0x2B2B2B, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF },
    { 0x2B2B2B, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0x2B2B2B, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF },
    { 0x2B2B2B, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0x2B2B2B, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF },
    { 0x2B2B2B, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0x2B2B2B, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF },
    { 0x2B2B2B, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0x2B2B2B, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF },
    { 0x2B2B2B, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0x2B2B2B, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF },
    { 0x2B2B2B, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0x2B2B2B, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF },
    { 0x2B2B2B, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0x2B2B2B, 0x2B2B2B, 0x2B2B2B, 0x2B2B2B, 0x2B2B2B, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF },
    { 0x2B2B2B, 0xFFFFFF, 0xFFFFFF, 0x2B2B2B, 0x2B2B2B, 0xFFFFFF, 0x2B2B2B, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF },
    { 0x2B2B2B, 0xFFFFFF, 0x2B2B2B, 0xFFFFFF, 0x2B2B2B, 0xFFFFFF, 0xFFFFFF, 0x2B2B2B, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF },
    { 0x2B2B2B, 0x2B2B2B, 0xFFFFFF, 0xFFFFFF, 0x2B2B2B, 0xFFFFFF, 0xFFFFFF, 0x2B2B2B, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF },
    { 0x2B2B2B, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0x2B2B2B, 0xFFFFFF, 0xFFFFFF, 0x2B2B2B, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF },
    { 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0x2B2B2B, 0xFFFFFF, 0xFFFFFF, 0x2B2B2B, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF },
    { 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0x2B2B2B, 0xFFFFFF, 0xFFFFFF, 0x2B2B2B, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF },
    { 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0x2B2B2B, 0xFFFFFF, 0xFFFFFF, 0x2B2B2B, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF },
    { 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0x2B2B2B, 0x2B2B2B, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF },
};

Monitor::Monitor(process_t orb, uintptr_t fb_addr, uint64_t width, uint64_t height, uint64_t pitch, uint64_t bpp, uint32_t xPos, uint32_t yPos) {
    
    this->xPos = xPos;
    this->yPos = yPos;

    framebuffer_t* _main = (framebuffer_t*) calloc(sizeof(framebuffer_t));
    framebuffer_t* _back = (framebuffer_t*) calloc(sizeof(framebuffer_t));

    _main->addr = fb_addr;
    _main->width = width;
    _main->height = height;
    _main->pitch = pitch;
    _main->bpp = bpp;
    _main->btpp = bpp / 8;
    _main->size = _main->pitch * height;

    _back->addr = calloc(_main->size);
    _back->width = width;
    _back->height = height;
    _back->pitch = _main->pitch;
    _back->bpp = _main->bpp;
    _back->btpp = _main->btpp;
    _back->size = _main->size;    

    main = _main;
    back = _back;

    this->background = new Window(orb, width, height, bpp, this->xPos, this->yPos);
}

uint32_t Monitor::getWidth() {
    return this->main->width;
}

uint32_t Monitor::getHeight() {
    return this->main->height;
}

Window* Monitor::getBackground() {
    return this->background;
}

void Monitor::setBackground(Window* w) {
    this->background->destroy();
    free(this->background);
    this->background = w;
}

void Monitor::move(uint32_t xPos, uint32_t yPos) {
    this->xPos = xPos;
    this->yPos = yPos;
    this->background->move(xPos, yPos);
}

void dynamicBlit(framebuffer_t* to, framebuffer_t* from, uint32_t x, uint32_t y, uint32_t monitorXoffset, uint32_t monitorYoffset) {
    blitFramebuffer(to, from, x, y);
}

void Monitor::update(vector_t* windows) {

    if (this->background == NULL) {
        memset(this->back->addr, 0x00, this->main->size);
    } else {
        memcpy(this->back->addr, this->background->getFramebuffer()->addr, this->main->size);
    }

    for (uint32_t i = 0; i < windows->length; i++) {
        Window* window = (Window*) vector_get(windows, i);
        if (window != NULL) if (window->isVisible()) {
            dynamicBlit(this->back, window->getFramebuffer(), window->getX(), window->getY(), this->xPos, this->yPos);
        }
    }

    DrawCursor(this->back, CursorPosition, MouseMask, CursorColor);

    memcpy(this->main->addr, this->back->addr, this->main->size);

}