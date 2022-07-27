#include <graphics/context.h>

Context::Context(framebuffer_t* framebuffer) {
    this->framebuffer = framebuffer;
    this->poses = vector_create(sizeof(pos_t));
    if (framebuffer->width >= framebuffer->height) {
        this->scale = this->framebuffer->width/412;
    } else {
        this->scale = this->framebuffer->height/412;
    }
}

void Context::putPixel(uint32_t x, uint32_t y, uint32_t colour) {
    if (this->pixelExist(x, y) == -1) return;
    uint8_t* fb = (uint8_t*) this->framebuffer->fb_addr;
    uint64_t index = x * this->framebuffer->btpp + y * this->framebuffer->pitch;
    *(uint32_t*)(fb + index) = colour;
}

int8_t Context::pixelExist(uint32_t x, uint32_t y) {
    if (x < 0 || y < 0) return -1;
    if (x > this->framebuffer->width || y > this->framebuffer->height) return -1;
    return 1;
}

uint32_t Context::getPixel(uint32_t x, uint32_t y) {
    uint8_t* fb = (uint8_t*) this->framebuffer->fb_addr;
    uint64_t index = x * this->framebuffer->btpp + y * this->framebuffer->pitch;
    return *(uint32_t*)(fb + index);
}

// ## path ##

void Context::auto_pos(bool _auto) {
    this->_auto = _auto;
}

pos_t* Context::get_pos(uint16_t index) {
    return (pos_t*) vector_get(this->poses, index);
}

void Context::scale_pos(bool _scaling) {
    this->_scaling = _scaling;
} 

uint16_t Context::get_scale() {
    return this->scale;
}

void Context::abs_pos(uint32_t x, uint32_t y) {
    this->x = x;
    this->y = y;
    if (this->_auto == true) {
        this->add_pos();
    }
}

void Context::rel_pos(uint32_t x, uint32_t y) {
    if (this->_scaling == true) {
        x = x * this->scale;
        y = y * this->scale;
    }
    this->x = this->x + x;
    this->y = this->y + y;
    if (this->_auto == true) {
        this->add_pos();
    }
}

void Context::add_pos() {
    pos_t* pos = (pos_t*) malloc(sizeof(pos_t));
    pos->x = this->x;
    pos->y = this->y;
    vector_push(this->poses, pos);
}

void Context::end_path() {
    if (this->poses->length > 0) {
        pos_t* to = (pos_t*) malloc(sizeof(pos_t));
        pos_t* from = (pos_t*) vector_get(this->poses, 0);
        to->x = from->x;
        to->y = from->y;
        vector_push(this->poses, to);
    }
}

void Context::draw(uint32_t colour) {
    if (this->_auto == true) {
        this->end_path();
    }
    for (uint64_t i = 0; i < this->poses->length-1; i++) {
        pos_t* pos1 = (pos_t*) vector_get(this->poses, i);
        pos_t* pos2 = (pos_t*) vector_get(this->poses, i+1);
        this->drawLine(pos1->x, pos1->y, pos2->x, pos2->y, colour);
    }
}

void Context::reset() {
    this->x = 0;
    this->y = 0;
    vector_clear(this->poses);
}

// ## absolute ##

void Context::fill(uint32_t x, uint32_t y, uint32_t colour) {
    this->fill(x, y, colour, colour);
} 

void Context::fill(uint32_t x, uint32_t y, uint32_t colour, uint32_t border) {
    uint32_t pixel = this->getPixel(x, y);
    if (pixel != colour && pixel != border && this->pixelExist(x, y) == 1) {
        this->putPixel(x, y, colour);
        this->fill(x+1, y, colour, border);
        this->fill(x, y+1, colour, border);
        this->fill(x-1, y, colour, border);
        this->fill(x, y-1, colour, border);
    }
} 

void Context::fillRect(uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint32_t colour) {

    uint8_t* fb = (uint8_t*) this->framebuffer->fb_addr;

    uint32_t _h = height+y;
    uint32_t _w = width+x;

    if (_h > this->framebuffer->height) {
        _h = this->framebuffer->height;
    }

    if (_w > this->framebuffer->width) {
        _w = this->framebuffer->width;
    }

    for (uint32_t h = y; h < _h; h++) {
        uint64_t ypos = h * this->framebuffer->pitch;
        for (uint32_t w = x; w < _w; w++) {
            uint64_t xpos = w * this->framebuffer->btpp;
            uint64_t index = ypos + xpos;
            *(uint32_t*)(fb + index) = colour;
        }
    }

}

void Context::drawLine(uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2, uint32_t colour) {

    if (x1 > this->framebuffer->width) {
        x1 = this->framebuffer->width;
    }

    if (y1 > this->framebuffer->height) {
        y1 = this->framebuffer->height;
    }

    if (x2 > this->framebuffer->width) {
        x2 = this->framebuffer->width;
    }

    if (y2 > this->framebuffer->height) {
        y2 = this->framebuffer->height;
    }

    int32_t dx = x2-x1;
    int32_t dy = y2-y1;

    int8_t sx = sgn(dx);
    int8_t sy = sgn(dy);

    int32_t x = x1;
    int32_t y = y1;

    int8_t isSwaped = 0;

    if(abs(dy) > abs(dx)) {
        int32_t tdx = dx;
        dx = dy;
        dy = tdx;
        isSwaped = 1;
    }

    int32_t p = 2*(abs(dy)) - abs(dx);

    this->putPixel(x, y, colour);

    for (int32_t i = 0; i < abs(dx); i++) {
        if (p < 0) {
            if (isSwaped == 0) {
                x = x + sx;
                this->putPixel(x, y, colour);
            } else {
                y = y+sy;
                this->putPixel(x, y, colour);
            }
            p = p + 2*abs(dy);
        } else {
            x = x+sx;
            y = y+sy;
            this->putPixel(x, y, colour);
            p = p + 2*abs(dy) - 2*abs(dx);
        }
    }

}

void Context::drawRect(uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint32_t colour) {
    this->drawLine(x, y, x+width, y, colour); // top
    this->drawLine(x, y+height, x+width, y+height, colour); // bottom
    this->drawLine(x, y, x, y+height, colour); // left
    this->drawLine(x+width, y, x+width, y+height, colour); // right
}

// ## frame buffer ##

void Context::swapTo(framebuffer_t* to) {
    memcpy((uintptr_t)to->fb_addr, (uintptr_t)this->framebuffer->fb_addr, this->framebuffer->fb_size);
}

void Context::swapFrom(framebuffer_t* from) {
    memcpy((uintptr_t)this->framebuffer->fb_addr, (uintptr_t)from->fb_addr, from->fb_size);
}

void Context::swapTo(Context* to) {
    this->swapTo(to->getFramebuffer());
}

void Context::swapFrom(Context* from) {
    this->swapFrom(from->getFramebuffer());
}

void blitFramebuffer(framebuffer_t* s1, framebuffer_t* s2, uint32_t x, uint32_t y) {

    uint64_t to = s1->fb_addr;
    uint64_t from = s2->fb_addr;

    to += x * s1->btpp + y * s1->pitch; // offset

    uint64_t num;

    if (s1->pitch < s2->pitch) {
        num = (uint64_t) s1->pitch;
    } else {
        num = (uint64_t) s2->pitch;
    } 

    for (uint64_t h = 0; h < s2->height && h+y < s1->height; h++) {
        memcpy((uintptr_t) to, (uintptr_t) from, num);
        to += s1->pitch;
        from += s2->pitch;
    }

}

void Context::blitTo(framebuffer_t* to, uint32_t x, uint32_t y) {
    blitFramebuffer(to, this->getFramebuffer(), x, y);
}

void Context::blitFrom(framebuffer_t* from, uint32_t x, uint32_t y) {
    blitFramebuffer(this->getFramebuffer(), from, x, y);
}

void Context::blitTo(Context* to, uint32_t x, uint32_t y) {
    this->blitTo(to->getFramebuffer(), x, y);
}

void Context::blitFrom(Context* from, uint32_t x, uint32_t y) {
    this->blitFrom(from->getFramebuffer(), x, y);
}

void Context::clear() {
    memset((uintptr_t) this->framebuffer->fb_addr, 0x00, this->framebuffer->fb_size);
} 

void Context::clear(uint32_t colour) {
    memset32((uintptr_t) this->framebuffer->fb_addr, colour, this->framebuffer->fb_size);
} 

framebuffer_t* Context::getFramebuffer() {
    return this->framebuffer;
}