#include <kot-ui++/component.h>

namespace Ui {
    void BoxDraw(Box_t* Box){
        memset32(Box->Cpnt->GetFramebuffer()->Buffer, Box->CurrentColor, Box->Cpnt->GetFramebuffer()->Size);
    }

    void BoxUpdate(Component* Cpnt){
        Box_t* Box = (Box_t*)Cpnt->ExternalData;
        if(Cpnt->IsFramebufferUpdate){
            // Draw
            Box->Style.Width = Cpnt->GetStyle()->Width;
            Box->Style.Height = Cpnt->GetStyle()->Height;

            BoxDraw(Box);
            Cpnt->IsFramebufferUpdate = false;
        }else if(Box->IsDrawUpdate){
            BoxDraw(Box);
            Box->IsDrawUpdate = false;
        }
        Cpnt->AbsolutePosition = {.x = Cpnt->Parent->AbsolutePosition.x + Box->Style.Position.x, .y = Cpnt->Parent->AbsolutePosition.y + Box->Style.Position.y};
        BlitFramebuffer(Cpnt->Parent->GetFramebuffer(), Cpnt->GetFramebuffer(), Box->Style.Position.x, Box->Style.Position.y);
        SetGraphicEventbuffer(Cpnt->UiCtx->EventBuffer, (uint64_t)Cpnt, Box->Style.Width, Box->Style.Height, Cpnt->AbsolutePosition.x, Cpnt->AbsolutePosition.y);
    }

    void BoxMouseEvent(class Component* Cpnt, bool IsHover, int64_t RelativePositionX, int64_t RelativePositionY, int64_t PositionX, int64_t PositionY, int64_t ZValue, uint64_t Status){
        if(IsHover){
            Box_t* Box = (Box_t*)Cpnt->ExternalData;
            if(Cpnt->UiCtx->FocusCpnt != Cpnt){
                if(Cpnt->UiCtx->FocusCpnt->MouseEvent){
                    Cpnt->UiCtx->FocusCpnt->MouseEvent(Cpnt->UiCtx->FocusCpnt, false, RelativePositionX, RelativePositionY, PositionX, PositionY, ZValue, Status);
                }
            }
            Cpnt->UiCtx->FocusCpnt = Box->Cpnt;
            if(Status & 0b1){
                Box->CurrentColor = Box->Style.HoverColor;
            }
            Box->IsDrawUpdate = true;
            Cpnt->UiCtx->Cpnt->Update();
        }else{
            Box_t* Box = (Box_t*)Cpnt->ExternalData;
            //Box->CurrentColor = Box->Style.BackgroundColor;
            Box->IsDrawUpdate = true;
            Cpnt->UiCtx->Cpnt->Update();            
        }
    }

    Box_t* Box(BoxStyle_t Style, UiContext* ParentUiContex){
        Box_t* Box = (Box_t*)malloc(sizeof(Box_t));
        memcpy(&Box->Style, &Style, sizeof(BoxStyle_t));
        Box->CurrentColor = Box->Style.BackgroundColor;
        Box->Cpnt = new Component({ .Width = Style.Width, .Height = Style.Height}, BoxUpdate, BoxMouseEvent, (uintptr_t)Box, ParentUiContex, true);
        return Box;
    }

}