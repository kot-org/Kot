#include "mouse.h"

Point_t CursorPosition;
Point_t CursorMaxPosition;

uint8_t Width;
uint8_t Height;

thread_t MouseRelativeInterrupt;

void InitializeCursor(){
    CursorPosition.x = 0;
    CursorPosition.y = 0;
    Sys_Createthread(Sys_GetProcess(), (uintptr_t)&CursorInterrupt, PriviledgeApp, NULL, &MouseRelativeInterrupt);
    BindMouseRelative(MouseRelativeInterrupt, false);
}

void CursorInterrupt(int64_t x, int64_t y, int64_t z, uint64_t status){
    /* Update X position */
    int64_t NewXCursorPosition = CursorPosition.x + x;
    if(NewXCursorPosition < 0){
        CursorPosition.x = 0;
    }else if(NewXCursorPosition > CursorMaxPosition.x){
        CursorPosition.x = CursorMaxPosition.x;
    }else{
        CursorPosition.x = NewXCursorPosition;
    }

    /* Update Y position */
    int64_t NewYCursorPosition = CursorPosition.y - y;
    if(NewYCursorPosition < 0){
        CursorPosition.y = 0;
    }else if(NewYCursorPosition > CursorMaxPosition.y){
        CursorPosition.y = CursorMaxPosition.y;
    }else{
        CursorPosition.y = NewYCursorPosition;
    }

    Width = CursorMaxPosition.x - CursorPosition.x;
    Height = CursorMaxPosition.y - CursorPosition.y;

    if(Width > CursorWidth){
        Width = CursorWidth;
    }

    if(Height > CursorHeight){
        Height = CursorHeight;
    }

    Sys_Event_Close();
}

void DrawCursor(Graphic::framebuffer_t* fb, uint8_t* Mask, uint32_t Color[CursorHeight][CursorWidth]) {    
    for(uint8_t y = 0; y < Height; y++) {
        for(uint8_t x = 0; x < Width; x++) {
            uint16_t bit = y * 16 + x;
            uint16_t byte = bit / 8;

            if(Mask[byte] & (0b10000000 >> (x % 8))){
                Graphic::putPixel(fb, CursorPosition.x + x, CursorPosition.y + y, Color[y][x]);
            }
        }
    }
}