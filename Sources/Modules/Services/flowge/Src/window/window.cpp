#include <window/window.h>

uint32_t autoX(screen_t* screen, uint32_t width) {
    return (screen->width - width) / 2;
}

uint32_t autoY(screen_t* screen, uint32_t height) {
    return (screen->height - height) / 2;
}

uint32_t autoWidth(screen_t* screen) {
    return 100;
}

uint32_t autoHeight(screen_t* screen) {
    return 100;
}

void swapWindowbuffer(screen_t* s1, screen_t* s2, uint32_t x, uint32_t y) {
    
    uint8_t *fb2 = (uint8_t*) s2->fb_addr;
    uint8_t *fb1 = (uint8_t*) s1->fb_addr;

    for (size_t h = 0; h < s2->height; h++) {
        size_t ypos_1 = (h + y) * s1->bps;
        size_t ypos_2 = (h + y) * s2->bps;
        for (size_t w = 0; w < s2->width; w++) {
            size_t xpos = (w + x) * s1->btpp;
            fb1[ypos_1+xpos] = fb2[ypos_2 + w * s2->btpp];
        }
    }

}

screen_t* createWindowbuffer(screen_t* screen, uint32_t width, uint32_t height) {
    screen_t* backbuffer = (screen_t *) malloc(sizeof(screen_t));
    size_t pitch = width * screen->btpp;
    size_t fb_size = pitch * height;
    backbuffer->fb_addr = (uint64_t)((uint8_t*) malloc(fb_size));
    backbuffer->fb_size = fb_size;
    backbuffer->width = width;
    backbuffer->height = height;
    backbuffer->bpp = screen->bpp;
    backbuffer->btpp = screen->btpp;
    backbuffer->bps = screen->bps;
    backbuffer->pitch = pitch;
    return backbuffer;
}

Window::Window(screen_t* screen) {
    this->width = autoWidth(screen);
    this->height = autoHeight(screen);
    this->x = autoX(screen, this->width);
    this->y = autoY(screen, this->height);
    this->framebuffer = createWindowbuffer(screen, this->width, this->height);
}

Window::Window(screen_t* screen, uint32_t width, uint32_t height) {
    this->width = 100;
    this->height = 100;
    this->x = autoX(screen, this->width);
    this->y = autoY(screen, this->height);
    this->framebuffer = createWindowbuffer(screen, this->width, this->height);
}

Window::Window(screen_t* screen, uint32_t x, uint32_t y, uint32_t width, uint32_t height) {
    this->x = x;
    this->y = y;
    this->width = width;
    this->height = height;
    this->framebuffer = createWindowbuffer(screen, this->width, this->height);
}

void Window::show() {
    this->_show = true;
}

void Window::hide() {
    this->_show = false;
}

screen_t* Window::getFramebuffer() {
    return this->framebuffer;
}