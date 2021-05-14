#include "graphics.h"

graphics* globalGraphics;

graphics::graphics(BootInfo* bootInfo) {
    framebuffer = bootInfo->framebuffer;
    PSF1_Font = bootInfo->psf1_Font;
    Color = 0xffffffff;
    CursorPosition = {0, 0};
}

void graphics::Print(const char* str){    
    char* chr = (char*)str;
    while(*chr != 0){
        PutChar(*chr, CursorPosition.X, CursorPosition.Y);
        CursorPosition.X+=8;
        if(CursorPosition.X + 8 > framebuffer->Width)
        {
            CursorPosition.X = 0;
            CursorPosition.Y += 16;
        }
        chr++;
    }
}

void graphics::PutChar(char chr, unsigned int xOff, unsigned int yOff){
    unsigned int* pixPtr = (unsigned int*)framebuffer->BaseAddressBackground;
    char* fontPtr = (char*)PSF1_Font->glyphBuffer + (chr * PSF1_Font->charsize) - 3; // -3 is to clean the reflect of the font
    for (unsigned long y = yOff; y < yOff + 16; y++){
        for (unsigned long x = xOff; x < xOff + 8; x++){
            if ((*fontPtr & (0b10000000 >> (x - xOff))) > 0){
                    *(unsigned int*)(pixPtr + x + (y * framebuffer->PixelsPerScanLine)) = Color;
                }

        }
        fontPtr++;
    }
}

void graphics::PutChar(char chr){
    PutChar(chr, CursorPosition.X, CursorPosition.Y);
    CursorPosition.X += 8;
    if(CursorPosition.X + 8 > framebuffer->Width){
        CursorPosition.X = 0;
        CursorPosition.Y += 16;
    }
}

void graphics::ClearChar(){
    if(CursorPosition.X == 0){
        CursorPosition.X = framebuffer->Width;
        CursorPosition.Y -= 16;
        if(CursorPosition.Y < 0){
            CursorPosition.Y = 0;
            CursorPosition.X = 0;
        }
    }
    unsigned int xOff = CursorPosition.X;
    unsigned int yOff = CursorPosition.Y;

    unsigned int* pixPtr = (unsigned int*)framebuffer->BaseAddressBackground;
    for (unsigned long y = yOff; y < yOff + 16; y++){
        for (unsigned long x = xOff - 8; x < xOff; x++){
            *(unsigned int*)(pixPtr + x + (y * framebuffer->PixelsPerScanLine)) = ClearColor;
        }
    }

    CursorPosition.X -= 8;
    if(CursorPosition.X < 0){
        CursorPosition.X = framebuffer->Width;
        CursorPosition.Y -= 16;
        if(CursorPosition.Y < 0){
            CursorPosition.Y = 0;
            CursorPosition.X = 0;
        }
    }
}

void graphics::Rectangle(int w, int h, int x, int y, int r, int g, int b) {
    for (int i = 0; i < h; i++) {
        for (int j = 0; j < w; j++) {
            Putpixel(j + x, i + y, r, g, b);
        }
    }
}

void graphics::Putpixel(int x, int y, int r, int g, int b) {
    unsigned char* screen = (unsigned char*)framebuffer->BaseAddressBackground;
    int where = (x + (y * framebuffer->Width)) * 4;
    screen[where] = b;          // BLUE
    screen[where + 1] = g;      // GREEN
    screen[where + 2] = r;      // RED
}

void graphics::Putpix(uint32_t x, uint32_t y, uint32_t color){
    *(uint32_t*)((uint64_t)framebuffer->BaseAddressBackground + (x*4) + (y * framebuffer->PixelsPerScanLine * 4)) = color;
}

uint32_t graphics::Getpix(uint32_t x, uint32_t y){
    return *(uint32_t*)((uint64_t)framebuffer->BaseAddressBackground + (x*4) + (y * framebuffer->PixelsPerScanLine * 4));
}

void graphics::Clear(){
    CursorPosition.X = 0;
    CursorPosition.Y = 0;
    uint64_t fbBase = (uint64_t)framebuffer->BaseAddressBackground;
    uint64_t bytesPerScanline = framebuffer->PixelsPerScanLine * 4;
    uint64_t fbHeight = framebuffer->Height;
    uint64_t fbSize = framebuffer->FrameBufferSize;

    for(int verticalScanLine = 0; verticalScanLine < fbHeight; verticalScanLine++){
        uint64_t pixPtrBase = fbBase + (bytesPerScanline * verticalScanLine);
        for(uint32_t* pixPtr = (uint32_t*)pixPtrBase; pixPtr < (uint32_t*)(pixPtrBase + bytesPerScanline); pixPtr++){
            *pixPtr = ClearColor;
        }
    }
}

void graphics::Next(){
    CursorPosition.X = 0;
    CursorPosition.Y += 16;
}

void graphics::Update(){
    memcpy(framebuffer->BaseAddress, framebuffer->BaseAddressBackground, framebuffer->FrameBufferSize);
}

void graphics::DrawOverlayMouseCursor(uint8_t* mouseCursor, Point position, uint32_t color){    
    int xMax = 16;
    int yMax = 16;
    int differenceX = framebuffer->Width - position.X;
    int differenceY = framebuffer->Height - position.Y;

    if(differenceX < 16) xMax = differenceX;
    if(differenceY < 16) yMax = differenceY;

    for(int y = 0; y < yMax; y++){
        for(int x = 0; x < xMax; x++){
            int bit = y * 16 + x;
            int byte = bit / 8;
            if((mouseCursor[byte] & (0b10000000 >> (x % 8)))){
                MouseCursorBuffer[x + y * 16] = Getpix(position.X + x, position.Y + y);
                Putpix(position.X + x, position.Y + y, color);
                MouseCursorBufferAfter[x + y * 16] = Getpix(position.X + x, position.Y + y);
            }
        }
    }
    MouseDrawn = true;
}

void graphics::ClearMouseCursor(uint8_t* mouseCursor, Point position){
    if(!MouseDrawn) return;
    int xMax = 16;
    int yMax = 16;
    int differenceX = framebuffer->Width - position.X;
    int differenceY = framebuffer->Height - position.Y;

    if(differenceX < 16) xMax = differenceX;
    if(differenceY < 16) yMax = differenceY;

    for(int y = 0; y < yMax; y++){
        for(int x = 0; x < xMax; x++){
            int bit = y * 16 + x;
            int byte = bit / 8;
            if((mouseCursor[byte] & (0b10000000 >> (x % 8)))){
                if(Getpix(position.X + x, position.Y + y) == MouseCursorBufferAfter[x + y * 16]){
                    Putpix(position.X + x, position.Y + y, MouseCursorBuffer[x + y * 16]);
                }                
            }
        }
    }
}