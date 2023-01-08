#include "monitor.h"

#include <kot++/printf.h>

Monitor::Monitor(process_t orb, uintptr_t fb_addr, uint64_t width, uint64_t height, uint64_t pitch, uint64_t bpp, uint32_t xPos, uint32_t yPos) {
    
    this->xPos = xPos;
    this->yPos = yPos;

    main = (framebuffer_t*) calloc(sizeof(framebuffer_t));
    back = (framebuffer_t*) calloc(sizeof(framebuffer_t));

    main->addr = fb_addr;
    main->width = width;
    main->height = height;
    main->pitch = pitch;
    main->bpp = bpp;
    main->btpp = bpp / 8;
    main->size = main->pitch * height;

    back->addr = calloc(main->pitch * height);
    back->width = width;
    back->height = height;
    back->pitch = main->pitch;
    back->bpp = main->bpp;
    back->btpp = main->btpp;
    back->size = main->size;

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

    DrawCursor(this->back, BitmapMask, PixelMap);

    memcpy(this->main->addr, this->back->addr, this->main->size);

}