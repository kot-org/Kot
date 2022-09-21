#include "monitor.h"

Monitor::Monitor(process_t orb, uintptr_t fb_addr, uint32_t width, uint32_t height, uint32_t xPos, uint32_t yPos) {
    
    this->xPos = xPos;
    this->yPos = yPos;

    framebuffer_t* _main = (framebuffer_t*) calloc(sizeof(framebuffer_t));
    framebuffer_t* _back = (framebuffer_t*) calloc(sizeof(framebuffer_t));

    _main->addr = fb_addr;
    _main->width = width;
    _main->height = height;
    _main->pitch = width * 4;
    _main->size = _main->pitch * height;

    _back->addr = calloc(_main->size);
    _back->width = width;
    _back->height = height;
    _back->pitch = _main->pitch;
    _back->size = _main->size;    

    main = _main;
    back = _back;

    this->background = new Window(orb, width, height, this->xPos, this->yPos);

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
            if (window->hasBorder()) {
                uint32_t w = window->getWidth();
                uint32_t h = window->getHeight();
                uint32_t x = window->getX();
                uint32_t y = window->getY();
                if (window->getFocusState() == ACTIVE) {
                    drawRect(this->back, x-1, y-1, w+1, h+1, 0x2B2B2B);
                } else {
                    drawRect(this->back, x-1, y-1, w+1, h+1, 0x4B4B4B);
                }
            }
        }
    }

    memcpy(this->main->addr, this->back->addr, this->main->size);

}