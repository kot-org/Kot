#include <kot-graphics/context.h>

void subSeqCircle(gctx_t* ctx, uint32_t xc, uint32_t yc, uint32_t x, uint32_t y, uint32_t colour);

gctx_t* CreateContext(uintptr_t fb_addr, uint32_t width, uint32_t height) {
    gctx_t* ctx = malloc(sizeof(gctx_t));
    ctx->fb_addr = fb_addr;
    ctx->poses = vector_create();
    ctx->width = width;
    ctx->height = height;
    ctx->pitch = width * ctx->btpp;
    ctx->fb_size = ctx->pitch * height;
    if (ctx->width >= ctx->height) {
        ctx->scale = ctx->width/412;
    } else {
        ctx->scale = ctx->height/412;
    }
    ctx->bpp = 32;
    ctx->btpp = 4;
    return ctx;
}

void putPixel(gctx_t* ctx, uint32_t x, uint32_t y, uint32_t colour) {
    if (pixelExist(ctx, x, y) == -1) return;
    uint64_t index = x * ctx->btpp + y * ctx->pitch;
    *(uint32_t*)((uint64_t) ctx->fb_addr + index) = colour;
}

int8_t pixelExist(gctx_t* ctx, uint32_t x, uint32_t y) {
    if (x < 0 || y < 0) return -1;
    if (x > ctx->width || y > ctx->height) return -1;
    return 1;
}

uint32_t getPixel(gctx_t* ctx, uint32_t x, uint32_t y) {
    uint64_t index = x * ctx->btpp + y * ctx->pitch;
    return *(uint32_t*)((uint64_t) ctx->fb_addr + index);
}

// ## path ##

void auto_pos(gctx_t* ctx, bool _auto) {
    ctx->_auto = _auto;
}

pos_t* get_pos(gctx_t* ctx, uint16_t index) {
    return (pos_t*) vector_get(ctx->poses, index);
}

void scale_pos(gctx_t* ctx, bool _scaling) {
    ctx->_scaling = _scaling;
} 

uint16_t get_scale(gctx_t* ctx) {
    return ctx->scale;
}

void abs_pos(gctx_t* ctx, uint32_t x, uint32_t y) {
    ctx->x = x;
    ctx->y = y;
    if (ctx->_auto == true) {
        add_pos(ctx);
    }
}

void rel_pos(gctx_t* ctx, uint32_t x, uint32_t y) {
    if (ctx->_scaling == true) {
        x = x * ctx->scale;
        y = y * ctx->scale;
    }
    ctx->x = ctx->x + x;
    ctx->y = ctx->y + y;
    if (ctx->_auto == true) {
        add_pos(ctx);
    }
}

void add_pos(gctx_t* ctx) {
    pos_t* pos = (pos_t*) malloc(sizeof(pos_t));
    pos->x = ctx->x;
    pos->y = ctx->y;
    vector_push(ctx->poses, pos);
}

void end_path(gctx_t* ctx) {
    if (ctx->poses->length > 0) {
        pos_t* to = (pos_t*) malloc(sizeof(pos_t));
        pos_t* from = (pos_t*) vector_get(ctx->poses, 0);
        to->x = from->x;
        to->y = from->y;
        vector_push(ctx->poses, to);
    }
}

void draw(gctx_t* ctx, uint32_t colour) {
    if (ctx->_auto == true) {
        end_path(ctx);
    }
    for (uint64_t i = 0; i < ctx->poses->length-1; i++) {
        pos_t* pos1 = (pos_t*) vector_get(ctx->poses, i);
        pos_t* pos2 = (pos_t*) vector_get(ctx->poses, i+1);
        drawLine(ctx, pos1->x, pos1->y, pos2->x, pos2->y, colour);
    }
}

void reset(gctx_t* ctx) {
    ctx->x = 0;
    ctx->y = 0;
    vector_clear(ctx->poses);
}

// ## absolute ##

void fill(gctx_t* ctx, uint32_t x, uint32_t y, uint32_t colour, uint32_t border) {
    uint32_t pixel = getPixel(ctx, x, y);
    if (pixel != colour && pixel != border && pixelExist(ctx, x, y) == 1) {
        putPixel(ctx, x, y, colour);
        fill(ctx, x+1, y, colour, border);
        fill(ctx, x, y+1, colour, border);
        fill(ctx, x-1, y, colour, border);
        fill(ctx, x, y-1, colour, border);
    }
} 

void fillRect(gctx_t* ctx, uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint32_t colour) {

    uint8_t* fb = (uint8_t*) ctx->fb_addr;

    uint32_t _h = height+y;
    uint32_t _w = width+x;

    if (_h > ctx->height) {
        _h = ctx->height;
    }

    if (_w > ctx->width) {
        _w = ctx->width;
    }

    for (uint32_t h = y; h < _h; h++) {
        uint64_t ypos = h * ctx->pitch;
        for (uint32_t w = x; w < _w; w++) {
            uint64_t xpos = w * ctx->btpp;
            uint64_t index = ypos + xpos;
            *(uint32_t*)(fb + index) = colour;
        }
    }

}

void subSeqCircle(gctx_t* ctx, uint32_t xc, uint32_t yc, uint32_t x, uint32_t y, uint32_t colour) {
    uint32_t w = ctx->width;
    uint32_t h = ctx->height;
    putPixel(ctx, xc+x+w/2, (h/2)-(yc+y), colour);
    putPixel(ctx, xc-x+w/2, (h/2)-(yc+y), colour);
    putPixel(ctx, xc+x+w/2, (h/2)-(yc-y), colour);
    putPixel(ctx, xc-x+w/2, (h/2)-(yc-y), colour);
    putPixel(ctx, xc+y+w/2, (h/2)-(yc+x), colour);
    putPixel(ctx, xc-y+w/2, (h/2)-(yc+x), colour);
    putPixel(ctx, xc+y+w/2, (h/2)-(yc-x), colour);
    putPixel(ctx, xc-y+w/2, (h/2)-(yc-x), colour);
}

void drawCircle(gctx_t* ctx, uint32_t xc, uint32_t yc, uint32_t r, uint32_t colour) {
    int x = 0, y = r;
    int d = 3 - 2 * r;
    subSeqCircle(ctx, xc, yc, x, y, colour);
    while (y >= x) {
        subSeqCircle(ctx, xc, yc, x, y, colour);
        x++;
        if (d > 0) {
            y--;
            d = d + 4 * (x - y) + 10;
        } else {
            d = d + 4 * x + 6;
        }
        subSeqCircle(ctx, xc, yc, x, y, colour);
    }
}

void drawLine(gctx_t* ctx, uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2, uint32_t colour) {

    if (x1 > ctx->width) {
        x1 = ctx->width;
    }

    if (y1 > ctx->height) {
        y1 = ctx->height;
    }

    if (x2 > ctx->width) {
        x2 = ctx->width;
    }

    if (y2 > ctx->height) {
        y2 = ctx->height;
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

    putPixel(ctx, x, y, colour);

    for (int32_t i = 0; i < abs(dx); i++) {
        if (p < 0) {
            if (isSwaped == 0) {
                x = x + sx;
                putPixel(ctx, x, y, colour);
            } else {
                y = y+sy;
                putPixel(ctx, x, y, colour);
            }
            p = p + 2*abs(dy);
        } else {
            x = x+sx;
            y = y+sy;
            putPixel(ctx, x, y, colour);
            p = p + 2*abs(dy) - 2*abs(dx);
        }
    }

}

void drawRect(gctx_t* ctx, uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint32_t colour) {
    drawLine(ctx, x, y, x+width, y, colour); // top
    drawLine(ctx, x, y+height, x+width, y+height, colour); // bottom
    drawLine(ctx, x, y, x, y+height, colour); // left
    drawLine(ctx, x+width, y, x+width, y+height, colour); // right
}

// ## frame buffer ##

void swapTo(gctx_t* ctx, uintptr_t to) {
    memcpy(to, ctx->fb_addr, ctx->fb_size);
}

void swapFrom(gctx_t* ctx, uintptr_t from) {
    memcpy(ctx->fb_addr, from, ctx->fb_size);
}

void swapToCtx(gctx_t* ctx) {
    swapTo(ctx, getFramebuffer(ctx));
}

void swapFromCtx(gctx_t* ctx) {
    swapFrom(ctx, getFramebuffer(ctx));
}

void clear(gctx_t* ctx) {
    memset(ctx->fb_addr, 0x00, ctx->fb_size);
} 

void clearColor(gctx_t* ctx, uint32_t colour) {
    memset32(ctx->fb_addr, colour, ctx->fb_size);
} 

uintptr_t getFramebuffer(gctx_t* ctx) {
    return ctx->fb_addr;
}