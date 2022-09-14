#pragma once

#include <kot/math.h>
#include <kot/utils/map.h>
#include <kot/cstring.h>

typedef struct {
    uint32_t x;
    uint32_t y;
} pos_t;

typedef struct {
    bool _auto;
    bool _scaling;
    uint32_t x;
    uint32_t y;
    vector_t* poses;
    uint16_t scale;
    uintptr_t fb_addr;
    size64_t fb_size;
    uint32_t width;
    uint32_t height;
    uint32_t pitch;
    uint8_t bpp;
    uint8_t btpp; // bpp / 8
} gctx_t;

gctx_t* CreateContext(uintptr_t fb_addr, uint32_t width, uint32_t height);

void putPixel(gctx_t* ctx, uint32_t x, uint32_t y, uint32_t colour);
int8_t pixelExist(gctx_t* ctx, uint32_t x, uint32_t y);
uint32_t getPixel(gctx_t* ctx, uint32_t x, uint32_t y);

void fillRect(gctx_t* ctx, uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint32_t colour);
void drawRect(gctx_t* ctx, uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint32_t colour);
void drawLine(gctx_t* ctx, uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2, uint32_t colour);

void drawCircle(gctx_t* ctx, uint32_t xc, uint32_t yc, uint32_t radius, uint32_t colour);

void fill(gctx_t* ctx, uint32_t x, uint32_t y, uint32_t colour, uint32_t border);

// path function

void draw(gctx_t* ctx, uint32_t colour);

void abs_pos(gctx_t* ctx, uint32_t x, uint32_t y);
void rel_pos(gctx_t* ctx, uint32_t x, uint32_t y);

void add_pos(gctx_t* ctx);
void end_path(gctx_t* ctx);
void reset(gctx_t* ctx);

pos_t* get_pos(gctx_t* ctx, uint16_t index);

void auto_pos(gctx_t* ctx, bool _auto);
void scale_pos(gctx_t* ctx, bool _scaling);

uint16_t get_scale(gctx_t* ctx);

// framebuffer function

void swapTo(gctx_t* ctx, uintptr_t to);
void swapFrom(gctx_t* ctx, uintptr_t from);
void swapToCtx(gctx_t* ctx);
void swapFromCtx(gctx_t* ctx);

void clear(gctx_t* ctx);
void clearColor(gctx_t* ctx, uint32_t colour);

uintptr_t getFramebuffer(gctx_t* ctx);

/* uint32_t getPitch(gctx_t* ctx) {
    return ctx->pitch;
} */