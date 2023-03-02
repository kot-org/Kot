#include <kot-graphics/utils.h>
#include <kot-graphics/context.h>

int8_t PixelExist(framebuffer_t* fb, uint32_t x, uint32_t y) {
    if (x < 0 || y < 0) return -1;
    if (x > fb->Width || y > fb->Height) return -1;
    return 1;
}

void inline BlendAlpha(uint32_t* pixel, uint32_t color){
    uint32_t a = color >> 24;    /* alpha */
    if(a == 0xff){
        *pixel = color;
    }else if(a != NULL){
        uint32_t rb = (((color & 0x00ff00ff) * a) + ((*pixel & 0x00ff00ff) * (0xff - a))) & 0xff00ff00;
        uint32_t g  = (((color & 0x0000ff00) * a) + ((*pixel & 0x0000ff00) * (0xff - a))) & 0x00ff0000;
        *pixel = (color & 0xff000000) | ((rb | g) >> 8);
    }
}

void PutPixel(framebuffer_t* fb, uint32_t x, uint32_t y, uint32_t color) {
    uint64_t index = x * 4 + y * fb->Pitch;
    BlendAlpha((uint32_t*)((uint64_t)fb->Buffer + index), color);
}

uint32_t GetPixel(framebuffer_t* fb, uint32_t x, uint32_t y) {
    uint64_t index = x * 4 + y * fb->Pitch;
    return *(uint32_t*)((uint64_t)fb->Buffer + index);
}

void BlitFramebuffer(framebuffer_t* To, framebuffer_t* From, uint64_t PositionX, uint64_t PositionY){
    uint64_t ToBuffer = (uint64_t)To->Buffer;
    uint64_t FromBuffer = (uint64_t)From->Buffer;

    ToBuffer += PositionX * To->Btpp + PositionY * To->Pitch; // offset

    uint64_t WidthCopy = From->Width;

    if(PositionX + WidthCopy >= To->Width){
        WidthCopy = To->Width - PositionX;
    }

    uint64_t HeightCopy = From->Height;

    if(PositionY + HeightCopy >= To->Height){
        HeightCopy = To->Height - PositionY;
    }

    uint64_t PitchCopy = WidthCopy * To->Btpp;

    for(uint64_t H = 0; H < HeightCopy; H++){
        memcpy((uintptr_t)ToBuffer, (uintptr_t)FromBuffer, PitchCopy);
        ToBuffer += To->Pitch;
        FromBuffer += From->Pitch;
    }
}


void BlitFramebufferRadius(framebuffer_t* to, framebuffer_t* from, uint64_t PositionX, uint64_t PositionY, uint64_t BorderRadius) {
    BlitFramebuffer(to, from, PositionX, PositionY);
    return;
    uint64_t ToBuffer = (uint64_t) to->Buffer;
    uint64_t FromBuffer = (uint64_t) from->Buffer;

    ToBuffer += PositionX * to->Btpp + PositionY * to->Pitch;
    uint64_t WidthCopy = from->Width;

    if (PositionX + WidthCopy >= to->Width) {
        WidthCopy = to->Width - PositionX;
    }

    uint64_t HeightCopy = from->Height;

    if (PositionY + HeightCopy >= to->Height) {
        HeightCopy = to->Height - PositionY;
    }

    uint64_t PitchCopy = WidthCopy * to->Btpp;

    uint64_t Ray = BorderRadius / 2;
    for (uint64_t h = 0; h < Ray && h < HeightCopy; h++) {
        uint64_t CircleH = h;
        uint64_t Height = (Ray - CircleH);
        int64_t LeftOffset = (int64_t)(Ray - sqrt(Ray*Ray-Height*Height)) * to->Btpp;
        if (LeftOffset < 0) {
            LeftOffset = 0;
        }
        memcpy((uintptr_t) (ToBuffer + LeftOffset), (uintptr_t) (FromBuffer + LeftOffset), PitchCopy - (LeftOffset * 2));
        ToBuffer += to->Pitch;
        FromBuffer += from->Pitch;
    }

    for (uint64_t h = Ray; h < HeightCopy - Ray && h < HeightCopy; h++) {
        memcpy((uintptr_t) ToBuffer, (uintptr_t) FromBuffer, PitchCopy);
        ToBuffer += to->Pitch;
        FromBuffer += from->Pitch;
    }

    for (uint64_t h = HeightCopy - Ray; h < HeightCopy; h++) {
        uint64_t CircleH = HeightCopy - h;
        uint64_t Height = (Ray - CircleH);
        int64_t LeftOffset = (int64_t)(Ray - sqrt(Ray*Ray-Height*Height)) * to->Btpp;
        if (LeftOffset < 0) {
            LeftOffset = 0;
        }
        memcpy((uintptr_t) (ToBuffer + LeftOffset), (uintptr_t) (FromBuffer + LeftOffset), PitchCopy - (LeftOffset * 2));
        ToBuffer += to->Pitch;
        FromBuffer += from->Pitch;
    }
}


void FillRect(framebuffer_t* fb, uint32_t x, uint32_t y, uint32_t Width, uint32_t Height, uint32_t color) {
    uint32_t _h = Height+y;
    uint32_t _w = Width+x;

    if (_h > fb->Height) { _h = fb->Height; }
    if (_w > fb->Width) { _w = fb->Width; }

    for (uint32_t h = y; h < _h; h++) {
        uint64_t YPosition = h * fb->Pitch;
        for (uint32_t w = x; w < _w; w++) {
            uint64_t XPosition = w * fb->Btpp;
            uint64_t index = YPosition + XPosition;
            *(uint32_t*)((uint64_t)fb->Buffer + index) = color;
            //blendAlpha((uintptr_t)((uint64_t)fb->Buffer + index), color);
        }
    }
}

void DrawLine(framebuffer_t* fb, uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2, uint32_t color) {

    if (x1 > fb->Width) { x1 = fb->Width; }
    if (y1 > fb->Height) { y1 = fb->Height; }
    if (x2 > fb->Width) { x2 = fb->Width; }
    if (y2 > fb->Height) { y2 = fb->Height; }

    if (x1 < 0) { x1 = 0; }
    if (y1 < 0) { y1 = 0; }
    if (x2 < 0) { x2 = 0; }
    if (y2 < 0) { y2 = 0; }

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

    PutPixel(fb, x, y, color);

    for (int32_t i = 0; i < abs(dx); i++) {
        if (p < 0) {
            if (isSwaped == 0) {
                x = x + sx;
                PutPixel(fb, x, y, color);
            } else {
                y = y+sy;
                PutPixel(fb, x, y, color);
            }
            p = p + 2*abs(dy);
        } else {
            x = x+sx;
            y = y+sy;
            PutPixel(fb, x, y, color);
            p = p + 2*abs(dy) - 2*abs(dx);
        }
    }

}

void DrawRect(framebuffer_t* fb, uint32_t x, uint32_t y, uint32_t Width, uint32_t Height, uint32_t color) {
    DrawLine(fb, x, y, x+Width, y, color); // top
    DrawLine(fb, x, y+Height, x+Width, y+Height, color); // bottom
    DrawLine(fb, x, y, x, y+Height, color); // left
    DrawLine(fb, x+Width, y, x+Width, y+Height, color); // right
}