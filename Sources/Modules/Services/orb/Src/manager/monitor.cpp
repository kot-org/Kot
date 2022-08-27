#include "monitor.h"

Monitor::Monitor(uintptr_t fb_addr, uint32_t width, uint32_t height, int32_t xPos, int32_t yPos) {
    
    this->fb_addr = fb_addr;
    this->width = width;
    this->height = height;
    this->xPos = xPos;
    this->yPos = yPos;

    this->pitch = width * this->btpp;
    this->fb_size = this->pitch * height;

    this->bb_addr = calloc(this->fb_size);

    this->background = new Window(this->width, this->height, this->xPos, this->yPos);

}

uint32_t Monitor::getWidth() {
    return this->width;
}

uint32_t Monitor::getHeight() {
    return this->height;
}

Window* Monitor::getBackground() {
    return this->background;
}

void Monitor::setBackground(Window* w) {
    this->background->destroy();
    free(this->background);
    this->background = w;
}

void Monitor::move(int32_t xPos, int32_t yPos) {
    this->xPos = xPos;
    this->yPos = yPos;
    this->background->move(xPos, yPos);
}

void dynamicBlit(uintptr_t to_addr, uint64_t to_pitch, uint32_t to_height, uintptr_t from_addr, uint64_t from_pitch, uint32_t from_height, int32_t x, int32_t y, int32_t x_pos, int32_t y_pos) {
    /* TODO */
}

void blit(uintptr_t to_addr, uint64_t to_pitch, uint32_t to_height, uintptr_t from_addr, uint64_t from_pitch, uint32_t from_height, uint32_t x, uint32_t y) {

    uint64_t to = (uint64_t) to_addr;
    uint64_t from = (uint64_t) from_addr;

    to += x * 4 + y * to_pitch; // offset

    uint64_t num;

    if (to_pitch < from_pitch) {
        num = to_pitch;
    } else {
        num = from_pitch;
    } 

    for (uint32_t h = 0; h < from_height && h + y < to_height; h++) {
        memcpy((uintptr_t) to, (uintptr_t) from, num);
        to += to_pitch;
        from += from_pitch;
    }

}

void Monitor::update(vector_t* windows) {

    if (this->background == NULL) {
        memset(this->bb_addr, 0x00, this->fb_size);
    } else {
        memcpy(this->bb_addr, this->background->getFramebuffer(), this->fb_size);
    }

    Context* ctx = new Context(this->bb_addr, this->width, this->height);

    for (uint32_t i = 0; i < windows->length; i++) {
        Window* window = (Window*) vector_get(windows, i);
        if (window != NULL) {
            blit(this->bb_addr, this->pitch, this->height, window->getFramebuffer(), window->getPitch(), window->getWidth(), window->getX(), window->getY());
            uint32_t w = window->getWidth();
            uint32_t h = window->getHeight();
            uint32_t x = window->getX();
            uint32_t y = window->getY();
            ctx->drawRect(x-1, y-1, w+1, h+1, 0x323232);
            ctx->drawLine(x+w-17, y+17, x+w-7, y+7, 0xffffff);
            ctx->drawLine(x+w-7, y+17, x+w-17, y+7, 0xffffff);
            ctx->drawRect(x+w-35, y+7, 10, 10, 0xffffff);
            ctx->drawLine(x+w-53, y+17, x+w-43, y+17, 0xffffff);
        }
    }

    memcpy(this->fb_addr, this->bb_addr, this->fb_size);

}