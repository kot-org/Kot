#include <kot-graphics/context.h>

void ctxSubSeqCircle(ctxg_t* ctx, uint32_t xc, uint32_t yc, uint32_t x, uint32_t y, uint32_t color);

ctxg_t* CreateGraphicContext(kot_framebuffer_t* fb) {
    ctxg_t* ctx = malloc(sizeof(ctxg_t));
    ctx->FbBase = fb->Buffer;
    ctx->poses = vector_create();
    ctx->Width = fb->Width;
    ctx->Height = fb->Height;
    ctx->Bpp = fb->Bpp;
    ctx->Btpp = fb->Btpp;
    ctx->Pitch = fb->Pitch;
    ctx->FbSize = fb->Size;
    
    if (ctx->Width >= ctx->Height) {
        ctx->scale = ctx->Width/412;
    } else {
        ctx->scale = ctx->Height/412;
    }
    return ctx;
}

void ctxPutPixel(ctxg_t* ctx, uint32_t x, uint32_t y, uint32_t color) {
    if (ctxPixelExist(ctx, x, y) == -1) return;
    uint64_t index = x * ctx->Btpp + y * ctx->Pitch;
    *(uint32_t*)((uint64_t)ctx->FbBase + index) = color;
    //blendAlpha(((uint64_t)ctx->FbBase + index), color);
}

int8_t ctxPixelExist(ctxg_t* ctx, uint32_t x, uint32_t y) {
    if (x < 0 || y < 0) return -1;
    if (x > ctx->Width || y > ctx->Height) return -1;
    return 1;
}

inline uint32_t ctxGetPixel(ctxg_t* ctx, uint32_t x, uint32_t y) {
    uint64_t index = x * ctx->Btpp + y * ctx->Pitch;
    return *(uint32_t*)((uint64_t) ctx->FbBase + index);
}

// ## path ##

void auto_pos(ctxg_t* ctx, bool _auto) {
    ctx->_auto = _auto;
}

pos_t* get_pos(ctxg_t* ctx, uint16_t index) {
    return (pos_t*) vector_get(ctx->poses, index);
}

void scale_pos(ctxg_t* ctx, bool _scaling) {
    ctx->_scaling = _scaling;
} 

uint16_t get_scale(ctxg_t* ctx) {
    return ctx->scale;
}

void abs_pos(ctxg_t* ctx, uint32_t x, uint32_t y) {
    ctx->x = x;
    ctx->y = y;
    if (ctx->_auto == true) {
        add_pos(ctx);
    }
}

void rel_pos(ctxg_t* ctx, uint32_t x, uint32_t y) {
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

void add_pos(ctxg_t* ctx) {
    pos_t* pos = (pos_t*) malloc(sizeof(pos_t));
    pos->x = ctx->x;
    pos->y = ctx->y;
    vector_push(ctx->poses, pos);
}

void end_path(ctxg_t* ctx) {
    if (ctx->poses->length > 0) {
        pos_t* to = (pos_t*) malloc(sizeof(pos_t));
        pos_t* from = (pos_t*) vector_get(ctx->poses, 0);
        to->x = from->x;
        to->y = from->y;
        vector_push(ctx->poses, to);
    }
}

void draw(ctxg_t* ctx, uint32_t color) {
    if (ctx->_auto == true) {
        end_path(ctx);
    }
    for (uint64_t i = 0; i < ctx->poses->length-1; i++) {
        pos_t* pos1 = (pos_t*) vector_get(ctx->poses, i);
        pos_t* pos2 = (pos_t*) vector_get(ctx->poses, i+1);
        ctxDrawLine(ctx, pos1->x, pos1->y, pos2->x, pos2->y, color);
    }
}

void resetCtx(ctxg_t* ctx) {
    ctx->x = 0;
    ctx->y = 0;
    vector_clear(ctx->poses);
}

// ## absolute ##

void ctxFill(ctxg_t* ctx, uint32_t x, uint32_t y, uint32_t color, uint32_t border) {
    uint32_t pixel = GetPixel(ctx, x, y);
    if (pixel != color && pixel != border && ctxPixelExist(ctx, x, y) == 1) {
        PutPixel(ctx, x, y, color);
        ctxFill(ctx, x+1, y, color, border);
        ctxFill(ctx, x, y+1, color, border);
        ctxFill(ctx, x-1, y, color, border);
        ctxFill(ctx, x, y-1, color, border);
    }
} 

void ctxFillRect(ctxg_t* ctx, uint32_t x, uint32_t y, uint32_t Width, uint32_t Height, uint32_t color) {

    uint8_t* fb = (uint8_t*) ctx->FbBase;

    uint32_t _h = Height+y;
    uint32_t _w = Width+x;

    if (_h > ctx->Height) {
        _h = ctx->Height;
    }

    if (_w > ctx->Width) {
        _w = ctx->Width;
    }

    for (uint32_t h = y; h < _h; h++) {
        uint64_t YPosition = h * ctx->Pitch;
        for (uint32_t w = x; w < _w; w++) {
            uint64_t XPosition = w * ctx->Btpp;
            uint64_t index = YPosition + XPosition;
            *(uint32_t*)((uint64_t)ctx->FbBase + index) = color;
            //blendAlpha(((uint64_t)ctx->FbBase + index), color);
        }
    }

}

void ctxSubSeqCircle(ctxg_t* ctx, uint32_t xc, uint32_t yc, uint32_t x, uint32_t y, uint32_t color) {
    uint32_t w = ctx->Width;
    uint32_t h = ctx->Height;
    ctxPutPixel(ctx, xc+x+w/2, (h/2)-(yc+y), color);
    ctxPutPixel(ctx, xc-x+w/2, (h/2)-(yc+y), color);
    ctxPutPixel(ctx, xc+x+w/2, (h/2)-(yc-y), color);
    ctxPutPixel(ctx, xc-x+w/2, (h/2)-(yc-y), color);
    ctxPutPixel(ctx, xc+y+w/2, (h/2)-(yc+x), color);
    ctxPutPixel(ctx, xc-y+w/2, (h/2)-(yc+x), color);
    ctxPutPixel(ctx, xc+y+w/2, (h/2)-(yc-x), color);
    ctxPutPixel(ctx, xc-y+w/2, (h/2)-(yc-x), color);
}

void ctxDrawCircle(ctxg_t* ctx, uint32_t xc, uint32_t yc, uint32_t r, uint32_t color) {
    int x = 0, y = r;
    int d = 3 - 2 * r;
    ctxSubSeqCircle(ctx, xc, yc, x, y, color);
    while (y >= x) {
        ctxSubSeqCircle(ctx, xc, yc, x, y, color);
        x++;
        if (d > 0) {
            y--;
            d = d + 4 * (x - y) + 10;
        } else {
            d = d + 4 * x + 6;
        }
        ctxSubSeqCircle(ctx, xc, yc, x, y, color);
    }
}

void ctxDrawLine(ctxg_t* ctx, uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2, uint32_t color) {

    if (x1 > ctx->Width) {
        x1 = ctx->Width;
    }

    if (y1 > ctx->Height) {
        y1 = ctx->Height;
    }

    if (x2 > ctx->Width) {
        x2 = ctx->Width;
    }

    if (y2 > ctx->Height) {
        y2 = ctx->Height;
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

    ctxPutPixel(ctx, x, y, color);

    for (int32_t i = 0; i < abs(dx); i++) {
        if (p < 0) {
            if (isSwaped == 0) {
                x = x + sx;
                ctxPutPixel(ctx, x, y, color);
            } else {
                y = y+sy;
                ctxPutPixel(ctx, x, y, color);
            }
            p = p + 2*abs(dy);
        } else {
            x = x+sx;
            y = y+sy;
            ctxPutPixel(ctx, x, y, color);
            p = p + 2*abs(dy) - 2*abs(dx);
        }
    }

}

void ctxDrawRect(ctxg_t* ctx, uint32_t x, uint32_t y, uint32_t Width, uint32_t Height, uint32_t color) {
    ctxDrawLine(ctx, x, y, x+Width, y, color); // top
    ctxDrawLine(ctx, x, y+Height, x+Width, y+Height, color); // bottom
    ctxDrawLine(ctx, x, y, x, y+Height, color); // left
    ctxDrawLine(ctx, x+Width, y, x+Width, y+Height, color); // right
}

// ## frame buffer ##

void swapTo(ctxg_t* ctx, uintptr_t to) {
    memcpy(to, ctx->FbBase, ctx->FbSize);
}

void swapFrom(ctxg_t* ctx, uintptr_t from) {
    memcpy(ctx->FbBase, from, ctx->FbSize);
}

void swapToCtx(ctxg_t* ctx) {
    swapTo(ctx, GetFramebuffer(ctx));
}

void swapFromCtx(ctxg_t* ctx) {
    swapFrom(ctx, GetFramebuffer(ctx));
}

void clear(ctxg_t* ctx) {
    memset(ctx->FbBase, 0x00, ctx->FbSize);
} 

void clearColor(ctxg_t* ctx, uint32_t color) {
    memset32(ctx->FbBase, color, ctx->FbSize);
} 

uintptr_t GetFramebuffer(ctxg_t* ctx) {
    return ctx->FbBase;
}