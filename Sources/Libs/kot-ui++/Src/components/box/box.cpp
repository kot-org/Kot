#include <kot-ui++/component.h>

namespace Ui {
    void BoxDraw(Component* Cpnt){
        Box_t* Box = (Box_t*)Cpnt->ExternalData;
        memset32(Box->Cpnt->GetFramebuffer()->Buffer, Box->Style.Color, Box->Cpnt->GetFramebuffer()->Size);
    }

    void BoxMouseEvent(class Component* Cpnt, int64_t RelativePositionX, int64_t RelativePositionY, int64_t PositionX, int64_t PositionY, int64_t ZValue, uint64_t Status){
        if(PositionX == -1 && PositionY == -1){
            Box_t* Box = (Box_t*)Cpnt->ExternalData;
            Box->Style.Color = 0xFF0000;
            BoxDraw(Cpnt);
            Cpnt->UiCtx->Cpnt->Update();            
        }else{
            Box_t* Box = (Box_t*)Cpnt->ExternalData;
            Box->Style.Color = 0x00FF00;
            if(Cpnt->UiCtx->FocusCpnt != Cpnt){
                if(Cpnt->UiCtx->FocusCpnt->MouseEvent){
                    Cpnt->UiCtx->FocusCpnt->MouseEvent(Cpnt->UiCtx->FocusCpnt, 0, 0, -1, -1, 0, 0);
                }
            }
            Cpnt->UiCtx->FocusCpnt = Box->Cpnt;
            BoxDraw(Cpnt);
            Cpnt->UiCtx->Cpnt->Update();
        }
    }

    Box_t* Box(BoxStyle_t Style, UiContext* ParentUiContex){
        Box_t* Box = (Box_t*)malloc(sizeof(Box_t));
        memcpy(&Box->Style, &Style, sizeof(BoxStyle_t));
        Box->Cpnt = new Component({ .Width = Style.Width, .Height = Style.Height}, BoxDraw, BoxMouseEvent, (uintptr_t)Box, ParentUiContex);
        BoxDraw(Box->Cpnt);
        return Box;
    }

}