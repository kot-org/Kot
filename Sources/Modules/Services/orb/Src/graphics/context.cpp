#include "context.h"

Context::Context(uintptr_t fb_addr, uint32_t width, uint32_t height) {
    this->fb_addr = fb_addr;
    this->poses = vector_create();
    this->width = width;
    this->height = height;
    this->pitch = this->width * this->btpp;
    this->fb_size = this->pitch * this->height;
    if (this->width >= this->height) {
        this->scale = this->width/412;
    } else {
        this->scale = this->height/412;
    }
}

void Context::putPixel(uint32_t x, uint32_t y, uint32_t colour) {
    if (this->pixelExist(x, y) == -1) return;
    uint8_t* fb = (uint8_t*) this->fb_addr;
    uint64_t index = x * this->btpp + y * this->pitch;
    *(uint32_t*)(fb + index) = colour;
}

int8_t Context::pixelExist(uint32_t x, uint32_t y) {
    if (x < 0 || y < 0) return -1;
    if (x > this->width || y > this->height) return -1;
    return 1;
}

uint32_t Context::getPixel(uint32_t x, uint32_t y) {
    uint8_t* fb = (uint8_t*) this->fb_addr;
    uint64_t index = x * this->btpp + y * this->pitch;
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

    uint8_t* fb = (uint8_t*) this->fb_addr;

    uint32_t _h = height+y;
    uint32_t _w = width+x;

    if (_h > this->height) {
        _h = this->height;
    }

    if (_w > this->width) {
        _w = this->width;
    }

    for (uint32_t h = y; h < _h; h++) {
        uint64_t ypos = h * this->pitch;
        for (uint32_t w = x; w < _w; w++) {
            uint64_t xpos = w * this->btpp;
            uint64_t index = ypos + xpos;
            *(uint32_t*)(fb + index) = colour;
        }
    }

}

void Context::subSeqCircle(uint32_t xc, uint32_t yc, uint32_t x, uint32_t y, uint32_t colour) {
    uint32_t w = this->width;
    uint32_t h = this->height;
    this->putPixel(xc+x+w/2, (h/2)-(yc+y), colour);
    this->putPixel(xc-x+w/2, (h/2)-(yc+y), colour);
    this->putPixel(xc+x+w/2, (h/2)-(yc-y), colour);
    this->putPixel(xc-x+w/2, (h/2)-(yc-y), colour);
    this->putPixel(xc+y+w/2, (h/2)-(yc+x), colour);
    this->putPixel(xc-y+w/2, (h/2)-(yc+x), colour);
    this->putPixel(xc+y+w/2, (h/2)-(yc-x), colour);
    this->putPixel(xc-y+w/2, (h/2)-(yc-x), colour);
}

void Context::drawCircle(uint32_t xc, uint32_t yc, uint32_t r, uint32_t colour) {
    int x = 0, y = r;
    int d = 3 - 2 * r;
    this->subSeqCircle(xc, yc, x, y, colour);
    while (y >= x) {
        this->subSeqCircle(xc, yc, x, y, colour);
        x++;
        if (d > 0) {
            y--;
            d = d + 4 * (x - y) + 10;
        } else {
            d = d + 4 * x + 6;
        }
        this->subSeqCircle(xc, yc, x, y, colour);
    }
}

void Context::drawLine(uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2, uint32_t colour) {

    if (x1 > this->width) {
        x1 = this->width;
    }

    if (y1 > this->height) {
        y1 = this->height;
    }

    if (x2 > this->width) {
        x2 = this->width;
    }

    if (y2 > this->height) {
        y2 = this->height;
    }

    if (x1 < 0) {
        x1 = 0;
    }

    if (y1 < 0) {
        y1 = 0;
    }

    if (x2 < 0) {
        x2 = 0;
    }

    if (y2 < 0) {
        y2 = 0;
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

void Context::swapTo(uintptr_t to) {
    memcpy(to, this->fb_addr, this->fb_size);
}

void Context::swapFrom(uintptr_t from) {
    memcpy(this->fb_addr, from, this->fb_size);
}

void Context::swapTo(Context* to) {
    this->swapTo(to->getFramebuffer());
}

void Context::swapFrom(Context* from) {
    this->swapFrom(from->getFramebuffer());
}

void Context::clear() {
    memset(this->fb_addr, 0x00, this->fb_size);
} 

void Context::clear(uint32_t colour) {
    memset32(this->fb_addr, colour, this->fb_size);
} 

uintptr_t Context::getFramebuffer() {
    return this->fb_addr;
}