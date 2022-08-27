#include "window.h"

Window::Window(process_t orb, uint32_t width, uint32_t height, int32_t xPos, int32_t yPos) {

    this->width = width;
    this->height = height;
    this->xPos = xPos;
    this->yPos = yPos;

    this->pitch = width * this->btpp;
    this->fb_size = this->pitch * height;

    uintptr_t address = getFreeAlignedSpace(this->fb_size);
    ksmem_t key = NULL;
    Sys_CreateMemoryField(orb, this->fb_size, &address, &key, MemoryFieldTypeShareSpaceRW);
    ksmem_t KeyShare = NULL;
    uint64_t Flags = NULL;
    Keyhole_SetFlag(&Flags, KeyholeFlagPresent, true);
    Sys_Keyhole_CloneModify(key, &KeyShare, NULL, Flags, PriviledgeApp);

    this->fb_addr = address;
    this->fb_key = KeyShare;

    this->owner = Sys_GetProcess();

    // clear window buffer
    memset(address, 0x00, this->fb_size);

}

uintptr_t Window::getFramebuffer() {
    return this->fb_addr;
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

void Window::destroy() {
    this->Sshow = false;
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
    return this->height;
}

uint32_t Window::getWidth() {
    return this->width;
}

uint32_t Window::getPitch() {
    return this->pitch;
}

int32_t Window::getX() {
    return this->xPos;
}

int32_t Window::getY() {
    return this->yPos;
}

process_t Window::getOwner() {
    return this->owner;
}