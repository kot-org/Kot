#include <window/window.h>

framebuffer_t* createWindowbuffer(framebuffer_t* screen, uint32_t width, uint32_t height) {
    framebuffer_t* buffer = (framebuffer_t *) malloc(sizeof(framebuffer_t));
    size_t pitch = width * screen->btpp;
    size_t fb_size = pitch * height;
    buffer->fb_addr = (uint64_t)((uint8_t*) malloc(fb_size));
    buffer->fb_size = fb_size;
    buffer->width = width;
    buffer->height = height;
    buffer->bpp = screen->bpp;
    buffer->btpp = screen->btpp;
    buffer->pitch = pitch;
    return buffer;
}

Window::Window(Context* from, uint32_t width, uint32_t height, uint32_t x, uint32_t y) 
: context(createWindowbuffer(from->getFramebuffer(), width, height)) {
    this->x = x;
    this->y = y;
    this->width = width;
    this->height = height;
}

void Window::show() {
    this->_show = true;
}

void Window::hide() {
    this->_show = false;
}

uint32_t Window::getWidth() {
    return this->width;
}

uint32_t Window::getHeight() {
    return this->height;
}

void Window::move(uint32_t x, uint32_t y) {
    this->x = x;
    this->y = y;
}

void Window::resize(uint32_t width, uint32_t height) {
    this->width = width;
    this->height = height;
    framebuffer_t* temp = createWindowbuffer(this->context.getFramebuffer(), this->width, this->height);
    this->context.blitTo(temp, 0, 0);
    free(this->context.getFramebuffer());
    this->context = Context(temp);
}

Context* Window::getContext() {
    return &this->context;
}

void Window::render(Context* to) {
    if (this->_show == true) {
        to->fillRect(this->x-1, this->y-20, this->width+1, 20, 0x414141);
        to->drawRect(this->x-1, this->y-20, this->width+1, 20, 0x414141);
        to->drawRect(this->x-1, this->y-1, this->width+1, this->height+1, 0x414141);
        to->drawLine(this->x+this->width-15, this->y-15, this->x+this->width-5, this->y-5, 0xffffff);
        to->drawLine(this->x+this->width-5, this->y-15, this->x+this->width-15, this->y-5, 0xffffff);
        if (this->title != NULL) {

        }
        this->getContext()->blitTo(to, this->x, this->y);
    }
}

char* Window::getTitle() {
    return this->title;
}

void Window::setTitle(char* title) {
    this->title = title;
}