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

Window::Window(Context* from, uint32_t width, uint32_t height, uint32_t x, uint32_t y) {
    this->x = x;
    this->y = y;
    if (width < 59) {
        width = 59;
    }
    if (height < 59) {
        height = 59;
    }
    this->width = width;
    this->height = height;
    this->context = new Context(createWindowbuffer(from->getFramebuffer(), width, height));
}

void Window::show() {
    this->_show = true;
}

void Window::hide() {
    this->_show = false;
}

void Window::showBorders() {
    this->_showBorders = true;
}

void Window::hideBorders() {
    this->_showBorders = false;
}

uint32_t Window::getWidth() {
    return this->width;
}

uint32_t Window::getHeight() {
    return this->height;
}

uint32_t Window::getX() {
    return this->x;
}

uint32_t Window::getY() {
    return this->y;
}

void Window::move(uint32_t x, uint32_t y) {
    this->x = x;
    this->y = y;
}

void Window::resize(uint32_t width, uint32_t height) {
    if (width < 59) {
        width = 59;
    }
    if (height < 59) {
        height = 59;
    }
    this->width = width;
    this->height = height;
    framebuffer_t* temp = createWindowbuffer(this->context->getFramebuffer(), this->width, this->height);
    this->context->blitTo(temp, 0, 0);
    free(this->context->getFramebuffer());
    this->context = new Context(temp);
}

Context* Window::getContext() {
    return this->context;
}

void Window::render(Context* to) {
    if (this->_show == true) {
        this->getContext()->blitTo(to, this->x, this->y);
        if (this->_showBorders) {
            to->drawRect(this->x-1, this->y-1, this->width+1, this->height+1, 0x323232);
            to->drawLine(this->x+this->width-17, this->y+17, this->x+this->width-7, this->y+7, 0xffffff);
            to->drawLine(this->x+this->width-7, this->y+17, this->x+this->width-17, this->y+7, 0xffffff);
            to->drawRect(this->x+this->width-35, this->y+7, 10, 10, 0xffffff);
            to->drawLine(this->x+this->width-53, this->y+17, this->x+this->width-43, this->y+17, 0xffffff);
        }
    }
}
