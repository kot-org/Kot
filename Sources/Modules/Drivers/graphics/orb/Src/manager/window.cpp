#include "window.h"

Window::Window(process_t orb, uint32_t width, uint32_t height, int32_t xPos, int32_t yPos) {

    this->fb = (framebuffer_t*) calloc(sizeof(framebuffer_t));

    this->fb->width = width;
    this->fb->height = height;
    this->xPos = xPos;
    this->yPos = yPos;

    this->fb->pitch = width * 4;
    this->fb->size = fb->pitch * height;

    uintptr_t address = getFreeAlignedSpace(this->fb->size);
    ksmem_t key = NULL;
    Sys_CreateMemoryField(orb, this->fb->size, &address, &key, MemoryFieldTypeShareSpaceRW);
    ksmem_t KeyShare = NULL;
    uint64_t Flags = NULL;
    Keyhole_SetFlag(&Flags, KeyholeFlagPresent, true);
    Sys_Keyhole_CloneModify(key, &KeyShare, NULL, Flags, PriviledgeApp);

    this->fb->addr = address;
    this->fb_key = KeyShare;

    this->owner = Sys_GetProcess();

    // clear window buffer
    memset(address, 0x00, this->fb->size);

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
    /* TODO */
}

void Window::move(int32_t xPos, int32_t yPos) {
    this->xPos = xPos;
    this->yPos = yPos;
}

uint32_t Window::getHeight() {
    return fb->height;
}

uint32_t Window::getWidth() {
    return fb->width;
}

int32_t Window::getX() {
    return xPos;
}

int32_t Window::getY() {
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