#include <kot-ui++/component.h>

#define Titlebar_Height 24
#define Titlebar_FontSize 16

namespace Ui {
    void CloseBtnEvent(Button_t* Button, ButtonEvent_t Type){
        if(Type & BUTTON_EVENT_TYPE_LEFT_CLICK){
            window_t* Window = (window_t*)Button->Style.ExternalData;
            ChangeVisibilityWindow(Window, false);
            // TODO close the process
        }
    }

    void SizeBtnEvent(Button_t* Button, ButtonEvent_t Type){
        if(Type & BUTTON_EVENT_TYPE_LEFT_CLICK){
            window_t* Window = (window_t*)Button->Style.ExternalData;
            WindowChangePosition(Window, 0, 0);
            ResizeWindow(Window, Window_Max_Size, Window_Max_Size);
            // TODO update framebuffer
        }
    }

    void HideBtnEvent(Button_t* Button, ButtonEvent_t Type){
        if(Type & BUTTON_EVENT_TYPE_LEFT_CLICK){
            window_t* Window = (window_t*)Button->Style.ExternalData;
            ChangeVisibilityWindow(Window, false);
            // TODO communicate with taskbar
        }
    }

    Titlebar_t* Titlebar(window_t* Window, char* Title, char* Icon, TitlebarStyle_t Style, Component* ParentCpnt) {
        Titlebar_t* Titlebar = (Titlebar_t*)malloc(sizeof(Titlebar_t));
        memcpy(&Titlebar->Style, &Style, sizeof(TitlebarStyle_t));
        Titlebar->MainBox = Box(
            {
                .G{
                    .Width = -100, 
                    .Height = Titlebar_Height, 
                },
                .BackgroundColor = Titlebar->Style.BackgroundColor, 
                .ClickColor = Titlebar->Style.BackgroundColor, 
                .HoverColor = Titlebar->Style.BackgroundColor, 
            }
        , ParentCpnt);

        Titlebar->MainFlexbox = Flexbox(
            {
                .Align{
                    .x = Layout::BETWEENHORIZONTAL,
                    .y = Layout::MIDDLE,
                },
                .G{
                    .Width = -100, 
                    .Height = -100, 
                },
            }
        , Titlebar->MainBox->Cpnt);
        
        Titlebar->Cpnt = Titlebar->MainFlexbox->Cpnt;

        Titlebar->Logo = Picturebox(Icon, _TGA, 
        {
            .Fit = PICTUREFILL, 
            .G{
                .Width = Titlebar_Height, 
                .Height = Titlebar_Height, 
            }
        }
        , Titlebar->Cpnt);

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
        , Titlebar->Cpnt);

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
        , Titlebar->Cpnt);

        Titlebar->CloseBtn = Button(
            {
                .BackgroundColor = Titlebar->Style.BackgroundColor, 
                .OnMouseEvent = CloseBtnEvent,
                .ExternalData = (uint64_t)Window,
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

        Titlebar->SizeBtn = Button(
            {
                .BackgroundColor = Titlebar->Style.BackgroundColor, 
                .OnMouseEvent = SizeBtnEvent,
                .ExternalData = (uint64_t)Window,
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
            , Titlebar->SizeBtn->Cpnt);

        Titlebar->HideBtn = Button(
            {
                .BackgroundColor = Titlebar->Style.BackgroundColor, 
                .OnMouseEvent = HideBtnEvent,
                .ExternalData = (uint64_t)Window,
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
            , Titlebar->HideBtn->Cpnt);
        return Titlebar;
    }

}