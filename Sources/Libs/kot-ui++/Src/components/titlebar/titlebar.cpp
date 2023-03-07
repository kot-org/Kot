#include <kot-ui++/component.h>
#include <kot-ui++/window.h>

#define Titlebar_Height 24
#define Titlebar_FontSize 16

namespace Ui {
    void TitlebarDraw(Titlebar_t* Titlebar){
        memset32(Titlebar->Cpnt->GetFramebuffer()->Buffer, Titlebar->CurrentColor, Titlebar->Cpnt->GetFramebuffer()->Size);
        Titlebar->Cpnt->IsRedraw = true;
    }

    void TitlebarUpdate(Component* Cpnt){
        Titlebar_t* Titlebar = (Titlebar_t*)Cpnt->ExternalData;
        if(Cpnt->IsFramebufferUpdate){
            TitlebarDraw(Titlebar);
            Cpnt->IsFramebufferUpdate = false;
        }else if(Cpnt->IsDrawUpdate){
            TitlebarDraw(Titlebar);
            Cpnt->IsDrawUpdate = false;
        }

        Cpnt->AbsolutePosition = {.x = (int64_t)(Cpnt->Parent->AbsolutePosition.x + Cpnt->Style->Position.x + Cpnt->Style->Margin.Left - Cpnt->Style->Margin.Right), .y = (int64_t)(Cpnt->Parent->AbsolutePosition.y + Cpnt->Style->Position.y + Cpnt->Style->Margin.Top - Cpnt->Style->Margin.Bottom)};
        SetGraphicEventbufferRadius(Cpnt->UiCtx->EventBuffer, (uint64_t)Cpnt, Cpnt->Style->Currentwidth, Cpnt->Style->Currentheight, Cpnt->AbsolutePosition.x, Cpnt->AbsolutePosition.y, Cpnt->Style->BorderRadius);
        Cpnt->Update();
        BlitFramebufferRadius(Cpnt->Parent->GetFramebuffer(), Cpnt->GetFramebuffer(), (int64_t)(Cpnt->Parent->FramebufferRelativePosition.x + Cpnt->Style->Position.x + Cpnt->Style->Margin.Left - Cpnt->Style->Margin.Right), (int64_t)(Cpnt->Parent->FramebufferRelativePosition.y + Cpnt->Style->Position.y + Cpnt->Style->Margin.Top - Cpnt->Style->Margin.Bottom), Cpnt->Style->BorderRadius);
    }

    void TitlebarMouseEvent(class Component* Cpnt, bool IsHover, int64_t RelativePositionX, int64_t RelativePositionY, int64_t PositionX, int64_t PositionY, int64_t ZValue, uint64_t Status){
        Titlebar_t* Titlebar = (Titlebar_t*)Cpnt->ExternalData;
        if(IsHover){
            if(Cpnt->UiCtx->FocusCpnt != NULL){
                if(Cpnt->UiCtx->FocusCpnt != Cpnt){
                    if(Cpnt->UiCtx->FocusCpnt->MouseEvent){
                        Cpnt->UiCtx->FocusCpnt->MouseEvent(Cpnt->UiCtx->FocusCpnt, false, RelativePositionX, RelativePositionY, PositionX, PositionY, ZValue, Status);
                    }
                }
            }
            Cpnt->UiCtx->FocusCpnt = Titlebar->Cpnt;

            if(Status & MOUSE_CLICK_LEFT){
                Titlebar->CurrentColor = Titlebar->Style.ClickColor;
                if(!((UiWindow::Window*)Titlebar->Window)->IsFullscreen){
                    if(Titlebar->IsMouseDrag){
                        WindowChangePosition(((UiWindow::Window*)Titlebar->Window)->Wid, ((UiWindow::Window*)Titlebar->Window)->Wid->Position.x + PositionX - Titlebar->MousePosition.x, ((UiWindow::Window*)Titlebar->Window)->Wid->Position.y + PositionY - Titlebar->MousePosition.y);
                    }
                    Titlebar->WindowInitialPosition = ((UiWindow::Window*)Titlebar->Window)->Wid->Position;
                    Titlebar->MousePosition = {.x = PositionX, .y = PositionY};
                    Titlebar->IsMouseDrag = true;
                }
            }else if(Status & MOUSE_CLICK_RIGHT){
                if(Titlebar->IsMouseDrag){
                    Titlebar->IsMouseDrag = false;   
                }
                Titlebar->CurrentColor = Titlebar->Style.ClickColor;
            }else{
                if(Titlebar->IsMouseDrag){
                    Titlebar->IsMouseDrag = false;   
                }
                Titlebar->CurrentColor = Titlebar->Style.HoverColor;
            }
            Cpnt->IsDrawUpdate = true;
        }else{
            Titlebar->CurrentColor = Titlebar->Style.BackgroundColor;
            Cpnt->IsDrawUpdate = true;           
        }
    }

    void CloseBtnEvent(Button_t* Button, ButtonStatus_t Type){
        if(Type & BUTTON_EVENT_TYPE_LEFT_CLICK){
            Titlebar_t* Titlebar = (Titlebar_t*)Button->Style.ExternalData;
            ((UiWindow::Window*)Titlebar->Window)->Close();
        }
    }

    void MaximizeBtnEvent(Button_t* Button, ButtonStatus_t Type){
        if(Type & BUTTON_EVENT_TYPE_LEFT_CLICK){
            Titlebar_t* Titlebar = (Titlebar_t*)Button->Style.ExternalData;
            ((UiWindow::Window*)Titlebar->Window)->Fullscreen();
        }
    }

    void MinimizeBtnEvent(Button_t* Button, ButtonStatus_t Type){
        if(Type & BUTTON_EVENT_TYPE_LEFT_CLICK){
            Titlebar_t* Titlebar = (Titlebar_t*)Button->Style.ExternalData;
            ((UiWindow::Window*)Titlebar->Window)->Hide();
        }
    }

    Titlebar_t* Titlebar(uintptr_t Window, char* Title, char* Icon, TitlebarStyle_t Style, Component* ParentCpnt) {
        Titlebar_t* Titlebar = (Titlebar_t*)malloc(sizeof(Titlebar_t));
        memcpy(&Titlebar->Style, &Style, sizeof(TitlebarStyle_t));
        Titlebar->CurrentColor = Titlebar->Style.BackgroundColor;
        Titlebar->Cpnt = new Component({                 
                .Width = -100, 
                .Height = Titlebar_Height, 
            }, TitlebarUpdate, TitlebarMouseEvent, (uintptr_t)Titlebar, ParentCpnt, true);

        Titlebar->Window = Window;

        Titlebar->MainFlexbox = Flexbox(
            {
                .Direction = Layout::ROW,
                .Align{
                    .x = Layout::BETWEENHORIZONTAL,
                    .y = Layout::MIDDLE,
                },
                .G{
                    .Width = -100, 
                    .Height = -100, 
                },
            }
        , Titlebar->Cpnt);

        Titlebar->Logo = Picturebox(Icon, _TGA, 
        {
            .Fit = PICTUREFILL, 
            .Transparency = true,
            .G{
                .Width = Titlebar_Height, 
                .Height = Titlebar_Height, 
            }
        }
        , Titlebar->MainFlexbox->Cpnt);

        Titlebar->Title = Label(
            {   
                .FontPath = "default-font.sfn",
                .Text = Title,
                .FontSize = Titlebar_FontSize,
                .ForegroundColor = Titlebar->Style.ForegroundColor, 
                .Align = Ui::TEXTALIGNCENTER,
                .AutoWidth = true,
                .AutoHeight = true,
            }
        , Titlebar->MainFlexbox->Cpnt);

        Titlebar->BtnBox = Box(
            {
                .G{
                    .Width = Titlebar_Height * 3, 
                    .Height = -100, 
                },
                .BackgroundColor = Titlebar->Style.BackgroundColor, 
                .ClickColor = Titlebar->Style.BackgroundColor, 
                .HoverColor = Titlebar->Style.BackgroundColor, 
            }
        , Titlebar->MainFlexbox->Cpnt);

        Titlebar->CloseBtn = Button(CloseBtnEvent, 
            {
                .BackgroundColor = Titlebar->Style.BackgroundColor, 
                .ExternalData = (uint64_t)Titlebar,
                .G{
                    .Width = Titlebar_Height, 
                    .Height = Titlebar_Height, 
                    .Align = RIGHT,
                    .BorderRadius = Titlebar_Height,
                },
            }
            , Titlebar->BtnBox->Cpnt);

        Picturebox_t* CloseImage = Picturebox("close.tga", _TGA, 
            {
                .Fit = PICTUREFILL,
                .Transparency = true,
                .G{
                    .Width = -100, 
                    .Height = -100, 
                    .IsHidden = false
                }
            }
            , Titlebar->CloseBtn->Cpnt);

        Titlebar->MaximizeBtn = Button(MaximizeBtnEvent, 
            {
                .BackgroundColor = Titlebar->Style.BackgroundColor, 
                .ExternalData = (uint64_t)Titlebar,
                .G{
                    .Width = Titlebar_Height, 
                    .Height = Titlebar_Height, 
                    .Align = RIGHT,
                    .BorderRadius = Titlebar_Height,
                }
            }
            , Titlebar->BtnBox->Cpnt);

        Picturebox_t* SizeImage = Picturebox("maximize.tga", _TGA, 
            {
                .Fit = PICTUREFILL, 
                .Transparency = true,
                .G{
                    .Width = -100, 
                    .Height = -100, 
                }
            }
            , Titlebar->MaximizeBtn->Cpnt);

        Titlebar->MinimizeBtn = Button(MinimizeBtnEvent, 
            {
                .BackgroundColor = Titlebar->Style.BackgroundColor, 
                .ExternalData = (uint64_t)Titlebar,
                .G{
                    .Width = Titlebar_Height,
                    .Height = Titlebar_Height,
                    .Align = RIGHT,
                    .BorderRadius = Titlebar_Height,
                }
            }
        , Titlebar->BtnBox->Cpnt);

        Picturebox_t* HideImage = Picturebox("minimize.tga", _TGA, 
            {
                .Fit = PICTUREFILL, 
                .Transparency = true,
                .G{
                    .Width = -100, 
                    .Height = -100, 
                }
            }
            , Titlebar->MinimizeBtn->Cpnt);
        return Titlebar;
    }

}
