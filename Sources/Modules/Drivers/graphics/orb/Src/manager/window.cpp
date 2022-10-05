#include "window.h"

void Window::newBuffer() {

    if (this->fb->addr != NULL && this->fb_key != NULL) {
        Sys_CloseMemoryField(this->orb, this->fb_key, this->fb->addr);
    }

    this->fb->pitch = this->fb->width * this->fb->btpp;
    this->fb->size = fb->pitch * this->fb->height;

    uintptr_t address = getFreeAlignedSpace(this->fb->size);
    ksmem_t key = NULL;
    Sys_CreateMemoryField(this->orb, this->fb->size, &address, &key, MemoryFieldTypeShareSpaceRW);
    ksmem_t KeyShare = NULL;
    uint64_t Flags = NULL;
    Keyhole_SetFlag(&Flags, KeyholeFlagPresent, true);
    Sys_Keyhole_CloneModify(key, &KeyShare, NULL, Flags, PriviledgeApp);

    this->fb->addr = address;
    this->fb_key = KeyShare;

    // clear window buffer
    memset(address, 0x00, this->fb->size);

}

Window::Window(process_t orb, uint64_t width, uint64_t height, uint64_t bpp, uint32_t xPos, uint32_t yPos) {

    this->xPos = xPos;
    this->yPos = yPos;

    this->fb = (framebuffer_t*) calloc(sizeof(framebuffer_t));

    this->fb->width = width;
    this->fb->height = height;

    this->fb->bpp = bpp;
    this->fb->btpp = bpp / 8;

    this->orb = orb;
    this->owner = Sys_GetProcess();

    newBuffer();

}

framebuffer_t* Window::getFramebuffer() {
    return this->fb;
}

ksmem_t Window::getFramebufferKey() {
    return this->fb_key;
}

void Window::show(bool val) {
    this->Sshow = val;
}

void Window::border(bool val) {
    this->Sborder = val;
}

bool Window::hasBorder() {
    return this->Sborder;
}

bool Window::isVisible() {
    return Sshow;
}

void Window::destroy() {
    Sshow = false;
    // todo: free shared framebuffer
}

void Window::resize(uint32_t width, uint32_t height) {
    this->fb->width = width;
    this->fb->height = height;
    newBuffer();
}

void Window::move(uint32_t xPos, uint32_t yPos) {
    this->xPos = xPos;
    this->yPos = yPos;
}

uint32_t Window::getHeight() {
    return fb->height;
}

uint32_t Window::getWidth() {
    return fb->width;
}

uint32_t Window::getBpp() {
    return fb->bpp;
}

uint32_t Window::getX() {
    return xPos;
}

uint32_t Window::getY() {
    return yPos;
}

process_t Window::getOwner() {
    return owner;
}

FocusState Window::getFocusState() {
    return focus_state;
}

void Window::setFocusState(FocusState focus_state) {
    this->focus_state = focus_state;
}