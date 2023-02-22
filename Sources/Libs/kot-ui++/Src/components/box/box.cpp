#include <kot-ui++/component.h>

namespace Ui {
    void BoxDraw(Box_t* Box){
        Box->Cpnt->IsRedraw = true;
        memset32(Box->Cpnt->GetFramebuffer()->Buffer, Box->CurrentColor, Box->Cpnt->GetFramebuffer()->Size);
    }

    void BoxUpdate(Component* Cpnt){
        Box_t* Box = (Box_t*)Cpnt->ExternalData;
        if(Cpnt->IsFramebufferUpdate){
            // Draw
            Box->Style.G.Width.Current = Cpnt->GetStyle()->Width.Current;
            Box->Style.G.Height.Current = Cpnt->GetStyle()->Height.Current;

            BoxDraw(Box);
            Cpnt->IsFramebufferUpdate = false;
        }else if(Box->IsDrawUpdate){
            BoxDraw(Box);
            Box->IsDrawUpdate = false;
        }
        Cpnt->AbsolutePosition = {.x = Cpnt->Parent->AbsolutePosition.x + Cpnt->Style->Position.x, .y = Cpnt->Parent->AbsolutePosition.y + Cpnt->Style->Position.y};
        BlitFramebuffer(Cpnt->Parent->GetFramebuffer(), Cpnt->GetFramebuffer(), Cpnt->Style->Position.x, Cpnt->Style->Position.y);
        SetGraphicEventbuffer(Cpnt->UiCtx->EventBuffer, (uint64_t)Cpnt, Box->Style.G.Width.Current, Box->Style.G.Height.Current, Cpnt->AbsolutePosition.x, Cpnt->AbsolutePosition.y);
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

            if(Status & MOUSE_CLICK_LEFT){
                Box->CurrentColor = Box->Style.ClickColor;
            }else if(Status & MOUSE_CLICK_RIGHT){
                Box->CurrentColor = Box->Style.ClickColor;
            }else{
                Box->CurrentColor = Box->Style.HoverColor;
            }

            Box->IsDrawUpdate = true;
        }else{
            Box_t* Box = (Box_t*)Cpnt->ExternalData;
            Box->CurrentColor = Box->Style.BackgroundColor;
            Box->IsDrawUpdate = true;           
        }
    }

    Box_t* Box(BoxStyle_t Style, Component* ParentCpnt){
        Box_t* Box = (Box_t*)malloc(sizeof(Box_t));
        memcpy(&Box->Style, &Style, sizeof(BoxStyle_t));
        Box->CurrentColor = Box->Style.BackgroundColor;
        Box->Cpnt = new Component({ Box->Style.G }, BoxUpdate, BoxMouseEvent, (uintptr_t)Box, ParentCpnt, true);
        return Box;
    }

}