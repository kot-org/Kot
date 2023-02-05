#include <mouse/mouse.h>

point_t CursorPosition;
point_t CursorMaxPosition;

uint64_t CursorWidth;
uint64_t CursorHeight;

int64_t Width;
int64_t Height;

uintptr_t PixelMap;
uintptr_t BitmapMask;

thread_t MouseRelativeInterrupt;

bool IsLastLeftClick = false;

void InitializeCursor(){
    file_t* KursorFile = fopen("d1:Bin/Kursors/darkDefault.kursor", "rb"); // todo: kursor settings

    if(KursorFile == NULL) {
        Printlog("[GRAPHICS/ORB] \033[0;31mERR:\033[0m Kursor file not found."); // todo: error log
        return;
    }

    fseek(KursorFile, 0, SEEK_END);
    uint64_t KursorFileSize = ftell(KursorFile);
	fseek(KursorFile, 0, SEEK_SET);

    KursorHeader* Header = (KursorHeader*) malloc(KursorFileSize);
    fread(Header, KursorFileSize, 1, KursorFile);

    Width = Header->Width;
    Height = Header->Height;
    uint64_t Pitch = Width * 4;

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

    free(Header);
    fclose(KursorFile);

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

    bool IsleftClick = status & 0x1;
    if(IsleftClick && IsLastLeftClick != IsleftClick){
        // Change focus
        for(uint64_t i = 0; i < Monitors->length; i++){
            monitorc* Monitor = (monitorc*)vector_get(Monitors, i);
            if(Monitor != NULL){
                if(IsBeetween(Monitor->XPosition, CursorPosition.x, Monitor->XMaxPosition) && IsBeetween(Monitor->YPosition, CursorPosition.y, Monitor->YMaxPosition)){
                    windowc* Window = (windowc*)GetEventData(Monitor->Eventbuffer, CursorPosition.x, CursorPosition.y);
                    if(Window){
                        Window->SetFocusState(true);
                    }
                }
            }
        }
    }

    IsLastLeftClick = IsleftClick;

    if(CurrentFocusWindow != NULL){
        arguments_t Parameters{
            .arg[0] = Window_Event_Mouse,               // Event type
            .arg[1] = (uint64_t)CursorPosition.x,       // X position
            .arg[2] = (uint64_t)CursorPosition.y,       // Y position
            .arg[3] = (uint64_t)z,                      // Z position (scroll)
            .arg[4] = status,                           // Status of buttons
        };
        Sys_Event_Trigger(CurrentFocusWindow->Event, &Parameters);
    }
    Sys_Event_Close();
}

void DrawCursor(framebuffer_t* fb, uintptr_t BitmapMask, uintptr_t PixelMap) {   
    uint32_t* Pixel = (uint32_t*)PixelMap;
    uint8_t* Mask = (uint8_t*)BitmapMask;
    
    uint32_t PixelPos;
    for(uint32_t y = 0; y < Height; y++) {
        for(uint32_t x = 0; x < Width; x++) {
            PixelPos = y * CursorWidth + x;

            if(BIT_CHECK(Mask[PixelPos / 8], PixelPos % 8))
                PutPixel(fb, CursorPosition.x + x, CursorPosition.y + y, Pixel[PixelPos]);
        }
    }
}