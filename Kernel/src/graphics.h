#pragma once

#include <stdint.h>
#include <stddef.h>
#include "boot/bootInfo.h"
#include "math.h"
#include "memory/heap.h"
#include "memory.h"

class graphics{
    public:
    /* define functions */
    graphics(BootInfo* bootInfo);
    void Print(const char* str);
    void PutChar(char chr, unsigned int xOff, unsigned int yOff);
    void PutChar(char chr);
    void ClearChar();
    void Rectangle(int w, int h, int x, int y, int r, int g, int b);
    void Putpixel(int x, int y, int r, int g, int b);
    void Putpix(uint32_t x, uint32_t y, uint32_t color);
    uint32_t Getpix(uint32_t x, uint32_t y);
    void Clear();
    void Next();
    void Update();
    void DrawOverlayMouseCursor(uint8_t* mouseCursor, Point position, uint32_t color);
    void ClearMouseCursor(uint8_t* mouseCursor, Point position);    

    /* define variables */
    Point               CursorPosition;
    Framebuffer*        framebuffer;
    void*               framebufferBackground;
    PSF_FONT*           PSF1_Font;
    unsigned int        Color;  
    unsigned int        ClearColor;  
    uint32_t            MouseCursorBuffer[16 * 16];
    uint32_t            MouseCursorBufferAfter[16 * 16];
    bool                MouseDrawn;
};

extern graphics* globalGraphics;