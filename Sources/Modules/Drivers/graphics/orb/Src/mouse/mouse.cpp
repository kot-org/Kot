#include "mouse.h"

Point_t CursorPosition;
Point_t CursorMaxPosition;

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

    Sys_Event_Close();
}

void DrawCursor(Graphic::framebuffer_t* fb, Point_t position, uint8_t* Mask, uint32_t Color[CursorHeight][CursorWidth]) {
    for(int y = 0; y < CursorHeight; y++) {
        for(int x = 0; x < CursorWidth; x++) {
            uint16_t bit = y * 16 + x;
            uint16_t byte = bit / 8;

            if(Mask[byte] & (0b10000000 >> (x % 8)))
                Graphic::putPixel(fb, position.x + x, position.y + y, Color[y][x]);

        }
    }
}