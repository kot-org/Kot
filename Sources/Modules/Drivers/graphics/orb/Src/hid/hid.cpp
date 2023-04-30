#include <hid/hid.h>

void KeyboardInterruptEntry(uint64_t KeyCode){
    hidc* Hid = (hidc*)Sys_GetExternalDataThread();
    Hid->KeyboardInterrupt(KeyCode);
    Sys_Event_Close();   
}

void CursorInterruptEntry(int64_t x, int64_t y, int64_t z, uint64_t status){
    hidc* Hid = (hidc*)Sys_GetExternalDataThread();
    Hid->CursorInterrupt(x, y, z, status);
    Sys_Event_Close();
}

hidc::hidc(orbc* Parent){
    Orb = Parent;

    file_t* KursorFile = fopen("d0:darkDefault.kursor", "rb"); // todo: kursor settings and use drive to store the cursor to d1:Bin/Kursors/

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
    CursorPosition.x = CursorMaxPosition.x / 2;
    CursorPosition.y = CursorMaxPosition.y / 2;

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

    Sys_CreateThread(Sys_GetProcess(), (uintptr_t)&CursorInterruptEntry, PriviledgeApp, (uint64_t)this, &MouseRelativeInterruptThread);

    BindMouseRelative(MouseRelativeInterruptThread, false);

    Sys_CreateThread(Sys_GetProcess(), (uintptr_t)&KeyboardInterruptEntry, PriviledgeApp, (uint64_t)this, &KeyboardInterruptThread);

    BindKeyboardEvent(KeyboardInterruptThread, false);
}

void hidc::KeyboardInterrupt(uint64_t KeyCode){
    if(CurrentFocusEvent != NULL){
        arguments_t Parameters{
            .arg[0] = Window_Event_Keyboard,            // Event type
            .arg[1] = (uint64_t)KeyCode,                // KeyCode
        };
        Sys_Event_Trigger(CurrentFocusEvent->Event, &Parameters);
    }    
}


void hidc::CursorInterrupt(int64_t x, int64_t y, int64_t z, uint64_t status){
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
        for(uint64_t i = 0; i < Orb->Render->Monitors->length; i++){
            monitorc* Monitor = (monitorc*)vector_get(Orb->Render->Monitors, i);
            if(Monitor != NULL){
                if(IsBeetween(Monitor->XPosition, CursorPosition.x, Monitor->XMaxPosition) && IsBeetween(Monitor->YPosition, CursorPosition.y, Monitor->YMaxPosition)){
                    hid_event_t* EventData = (hid_event_t*)GetEventData(Monitor->Eventbuffer, CursorPosition.x, CursorPosition.y);
                    CurrentFocusEvent = EventData;
                    if(EventData){
                        if(EventData->ParentType == MOUSE_EVENT_PARENT_TYPE_WINDOW){
                            ((windowc*)EventData->Parent)->SetFocusState(true);
                        }
                    }
                }
            }
        }
    }

    IsLastLeftClick = IsleftClick;

    if(CurrentFocusEvent != NULL){
        arguments_t Parameters{
            .arg[0] = Window_Event_Mouse,               // Event type
            .arg[1] = (uint64_t)CursorPosition.x,       // X position
            .arg[2] = (uint64_t)CursorPosition.y,       // Y position
            .arg[3] = (uint64_t)z,                      // Z position (scroll)
            .arg[4] = status,                           // Status of buttons
        };
        Sys_Event_Trigger(CurrentFocusEvent->Event, &Parameters);
    }
}

void hidc::DrawCursor(framebuffer_t* fb){   
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