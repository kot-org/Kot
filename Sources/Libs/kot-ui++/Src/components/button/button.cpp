#include <kot-ui++/component.h>

namespace Ui {
    void ButtonDraw(Button_t* Button){
        memset32(Button->Cpnt->GetFramebuffer()->Buffer, Button->CurrentColor, Button->Cpnt->GetFramebuffer()->Size);
    }

    void ButtonUpdate(Component* Cpnt){
        Button_t* Button = (Button_t*)Cpnt->ExternalData;
        if(Cpnt->IsFramebufferUpdate){
            // Draw
            Button->Style.Width = Cpnt->GetStyle()->Width;
            Button->Style.Height = Cpnt->GetStyle()->Height;

            ButtonDraw(Button);
            Cpnt->IsFramebufferUpdate = false;
        }else if(Button->IsDrawUpdate){
            ButtonDraw(Button);
            Button->IsDrawUpdate = false;
        }
        Cpnt->AbsolutePosition = {.x = Cpnt->Parent->AbsolutePosition.x + Cpnt->Style->Position.x, .y = Cpnt->Parent->AbsolutePosition.y + Cpnt->Style->Position.y};
        BlitFramebuffer(Cpnt->Parent->GetFramebuffer(), Cpnt->GetFramebuffer(), Cpnt->Style->Position.x, Cpnt->Style->Position.y);
        SetGraphicEventbuffer(Cpnt->UiCtx->EventBuffer, (uint64_t)Cpnt, Button->Style.Width, Button->Style.Height, Cpnt->AbsolutePosition.x, Cpnt->AbsolutePosition.y);
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

            if(Status & MOUSE_CLICK_LEFT){
                Button->CurrentColor = Button->Style.ClickColor;
                Button->Style.OnClick(Button, ButtonEventTypeLeftClick);
                Button->IsDrawUpdate = true;
            }else if(Status & MOUSE_CLICK_RIGHT){
                Button->CurrentColor = Button->Style.ClickColor;
                Button->Style.OnClick(Button, ButtonEventTypeRightClick);
                Button->IsDrawUpdate = true;
            }else if(Status & MOUSE_CLICK_MIDDLE){
                Button->Style.OnClick(Button, ButtonEventTypeMiddleClick);
                Button->CurrentColor = Button->Style.HoverColor;
                Button->IsDrawUpdate = true;
            }else{
                Button->Style.OnClick(Button, ButtonEventTypeHover);
                Button->CurrentColor = Button->Style.HoverColor;
                Button->IsDrawUpdate = true;               
            }
        }else{
            Button_t* Button = (Button_t*)Cpnt->ExternalData;
            Button->CurrentColor = Button->Style.BackgroundColor;
            Button->IsDrawUpdate = true;
            Button->Style.OnClick(Button, ButtonEventTypeUnfocus);   
        }
    }

    Button_t* Button(ButtonStyle_t Style, UiContext* ParentUiContex){
        Button_t* Button = (Button_t*)malloc(sizeof(Button_t));
        memcpy(&Button->Style, &Style, sizeof(ButtonStyle_t));
        Button->CurrentColor = Button->Style.BackgroundColor;
        Button->Cpnt = new Component({ .Width = Style.Width, .Height = Style.Height, .IsVisible = Style.IsVisible, .Position = {.x = Style.Position.x, .y = Style.Position.y}}, ButtonUpdate, ButtonMouseEvent, (uintptr_t)Button, ParentUiContex, true);
        return Button;
    }

}