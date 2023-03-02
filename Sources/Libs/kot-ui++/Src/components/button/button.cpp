#include <kot-ui++/component.h>
#include <kot/stdio.h>

namespace Ui {
    void ButtonDraw(Button_t* Button){
        memset32(Button->Cpnt->GetFramebuffer()->Buffer, Button->CurrentColor, Button->Cpnt->GetFramebuffer()->Size);
        Button->Cpnt->IsRedraw = true;
    }

    void ButtonUpdate(Component* Cpnt){
        Button_t* Button = (Button_t*)Cpnt->ExternalData;
        if(Cpnt->IsFramebufferUpdate){
            // Draw
            Button->Style.G.Currentwidth = Cpnt->GetStyle()->Currentwidth;
            Button->Style.G.Currentheight = Cpnt->GetStyle()->Currentheight;

            ButtonDraw(Button);
            Cpnt->IsFramebufferUpdate = false;
        }else if(Cpnt->IsDrawUpdate){
            ButtonDraw(Button);
            Cpnt->IsDrawUpdate = false;
        }
        Cpnt->AbsolutePosition = {.x = (int64_t)(Cpnt->Parent->AbsolutePosition.x + Cpnt->Style->Position.x + Cpnt->Style->Margin.Left - Cpnt->Style->Margin.Right), .y = (int64_t)(Cpnt->Parent->AbsolutePosition.y + Cpnt->Style->Position.y + Cpnt->Style->Margin.Top - Cpnt->Style->Margin.Bottom)};
        SetGraphicEventbufferRadius(Cpnt->UiCtx->EventBuffer, (uint64_t)Cpnt, Button->Style.G.Currentwidth, Button->Style.G.Currentheight, Cpnt->AbsolutePosition.x, Cpnt->AbsolutePosition.y, Cpnt->Style->BorderRadius);
        Cpnt->Update();
        BlitFramebufferRadius(Cpnt->Parent->GetFramebuffer(), Cpnt->GetFramebuffer(), (int64_t)(Cpnt->Parent->FramebufferRelativePosition.x + Cpnt->Style->Position.x + Cpnt->Style->Margin.Left - Cpnt->Style->Margin.Right), (int64_t)(Cpnt->Parent->FramebufferRelativePosition.y + Cpnt->Style->Position.y + Cpnt->Style->Margin.Top - Cpnt->Style->Margin.Bottom), Cpnt->Style->BorderRadius);
    }

    void ButtonMouseEvent(class Component* Cpnt, bool IsHover, int64_t RelativePositionX, int64_t RelativePositionY, int64_t PositionX, int64_t PositionY, int64_t ZValue, uint64_t Status){
        if(IsHover){
            Button_t* Button = (Button_t*)Cpnt->ExternalData;
            if(Cpnt->UiCtx->FocusCpnt != Cpnt){
                if(Cpnt->UiCtx->FocusCpnt->MouseEvent){
                    Cpnt->UiCtx->FocusCpnt->MouseEvent(Cpnt->UiCtx->FocusCpnt, false, RelativePositionX, RelativePositionY, PositionX, PositionY, ZValue, Status);
                }
            }
            Cpnt->UiCtx->FocusCpnt = Button->Cpnt;

            if(Status & (MOUSE_CLICK_LEFT | MOUSE_CLICK_RIGHT | MOUSE_CLICK_MIDDLE)){
                Button->CurrentColor = Button->Style.ClickColor;
                Button->Style.OnMouseEvent(Button, BUTTON_EVENT_TYPE_HOVER | ((Status & MOUSE_CLICK_LEFT) << 1) | ((Status & MOUSE_CLICK_RIGHT) << 2) | ((Status & MOUSE_CLICK_MIDDLE) << 3));
                Cpnt->IsDrawUpdate = true;
            }else{
                Button->Style.OnMouseEvent(Button, BUTTON_EVENT_TYPE_HOVER);
                Button->CurrentColor = Button->Style.HoverColor;
                Cpnt->IsDrawUpdate = true;               
            }
        }else{
            Button_t* Button = (Button_t*)Cpnt->ExternalData;
            Button->CurrentColor = Button->Style.BackgroundColor;
            Cpnt->IsDrawUpdate = true;
            Button->Style.OnMouseEvent(Button, BUTTON_EVENT_TYPE_UNFOCUS);   
        }
    }

    Button_t* Button(ButtonStyle_t Style, Component* ParentCpnt){
        Button_t* Button = (Button_t*)malloc(sizeof(Button_t));
        memcpy(&Button->Style, &Style, sizeof(ButtonStyle_t));
        Button->CurrentColor = Button->Style.BackgroundColor;
        Button->Cpnt = new Component(Style.G, ButtonUpdate, ButtonMouseEvent, (uintptr_t)Button, ParentCpnt, true);
        return Button;
    }

}