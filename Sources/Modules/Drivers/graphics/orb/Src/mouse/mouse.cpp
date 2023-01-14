#include <mouse/mouse.h>

Point_t CursorPosition;
Point_t CursorMaxPosition;

uint64_t CursorWidth;
uint64_t CursorHeight;

int64_t Width;
int64_t Height;
uint64_t Pitch;

uintptr_t PixelMap;
uintptr_t BitmapMask;

thread_t MouseRelativeInterrupt;

void InitializeCursor(){
    srv_system_callback_t* KursorFile = (srv_system_callback_t*) Srv_System_ReadFileInitrd("darkDefault.kursor", true);
    KursorHeader* Header = (KursorHeader*) KursorFile->Data;

    Width = Header->Width;
    Height = Header->Height;
    Pitch = Width * 4;

    CursorWidth = Width;
    CursorHeight = Height;
    CursorPosition.x = 0;
    CursorPosition.y = 0;

    uintptr_t PixelMapTmp = (uintptr_t) ((uint64_t)Header + Header->PixelMapOffset);
    size64_t PixelMapSize = Height * Pitch;
    PixelMap = malloc(PixelMapSize);
    memcpy(PixelMap, PixelMapTmp, PixelMapSize);

    uintptr_t BitmapMaskTmp = (uintptr_t) ((uint64_t)Header + Header->BitmapMaskOffset);
    size64_t BitmapMaskSize = DivideRoundUp(Height * Pitch, 8);
    BitmapMask = malloc(BitmapMaskSize);
    memcpy(BitmapMask, BitmapMaskTmp, BitmapMaskSize);

    free(KursorFile);

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

    int64_t NewWidth = CursorMaxPosition.x - CursorPosition.x;
    int64_t NewHeight = CursorMaxPosition.y - CursorPosition.y;

    if(NewWidth > CursorWidth){
        Width = CursorWidth;
    }else if(NewWidth < 0){
        Width = 0;
    }else{
        Width = NewWidth;
    }

    if(NewHeight > CursorHeight){
        Height = CursorHeight;
    }else if(NewHeight < 0){
        Height = 0;
    }else{
        Height = NewHeight;
    }

    Sys_Event_Close();
}

void DrawCursor(framebuffer_t* fb, uintptr_t BitmapMask, uintptr_t PixelMap) {   
    uint32_t* Pixel = (uint32_t*) PixelMap;
    uint8_t* Mask = (uint8_t*) BitmapMask;
    
    int i = 0;
    for(uint64_t y = 0; y < Height; y++) {
        for(uint64_t x = 0; x < Width; x++) {
            uint64_t bit = y * CursorWidth + x;
            uint64_t byte = bit / 8;

            if(Mask[byte] & (0b10000000 >> (x % 8))){
                PutPixel(fb, CursorPosition.x + x, CursorPosition.y + y, Pixel[i]);
            }

            i++;
        }
    }
}