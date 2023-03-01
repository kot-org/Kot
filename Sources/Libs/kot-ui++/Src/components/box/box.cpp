#include <kot-ui++/component.h>

namespace Ui {
    void BoxDraw(Box_t* Box){
        memset32(Box->Cpnt->GetFramebuffer()->Buffer, Box->CurrentColor, Box->Cpnt->GetFramebuffer()->Size);
        Box->Cpnt->IsRedraw = true;
    }

    void BoxUpdate(Component* Cpnt){
        Box_t* Box = (Box_t*)Cpnt->ExternalData;
        if(Cpnt->IsFramebufferUpdate){
            // Draw
            Box->Style.G.Currentwidth = Cpnt->GetStyle()->Currentwidth;
            Box->Style.G.Currentheight = Cpnt->GetStyle()->Currentheight;

            BoxDraw(Box);
            Cpnt->IsFramebufferUpdate = false;
        }else if(Box->IsDrawUpdate){
            BoxDraw(Box);
            Box->IsDrawUpdate = false;
        }

        Cpnt->AbsolutePosition = {.x = (int64_t)(Cpnt->Parent->AbsolutePosition.x + Cpnt->Style->Position.x + Cpnt->Style->Margin.Left - Cpnt->Style->Margin.Right), .y = (int64_t)(Cpnt->Parent->AbsolutePosition.y + Cpnt->Style->Position.y + Cpnt->Style->Margin.Top - Cpnt->Style->Margin.Bottom)};
        SetGraphicEventbufferRadius(Cpnt->UiCtx->EventBuffer, (uint64_t)Cpnt, Box->Style.G.Currentwidth, Box->Style.G.Currentheight, Cpnt->AbsolutePosition.x, Cpnt->AbsolutePosition.y, Cpnt->Style->BorderRadius);
        Cpnt->Update();
        BlitFramebufferRadius(Cpnt->Parent->GetFramebuffer(), Cpnt->GetFramebuffer(), (int64_t)(Cpnt->Parent->FramebufferRelativePosition.x + Cpnt->Style->Position.x + Cpnt->Style->Margin.Left - Cpnt->Style->Margin.Right), (int64_t)(Cpnt->Parent->FramebufferRelativePosition.y + Cpnt->Style->Position.y + Cpnt->Style->Margin.Top - Cpnt->Style->Margin.Bottom), Cpnt->Style->BorderRadius);
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