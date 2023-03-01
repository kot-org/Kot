#include <kot-ui++/component.h>

#define Titlebar_Height 24
#define Titlebar_FontSize 16

namespace Ui {
    void CloseBtnEvent(Button_t* Button, ButtonEvent_t Type){
        Printlog("close");
    }

    void SizeBtnEvent(Button_t* Button, ButtonEvent_t Type){
        Printlog("size");
    }

    void HideBtnEvent(Button_t* Button, ButtonEvent_t Type){
        Printlog("hide");
    }

    Titlebar_t* Titlebar(char* Title, char* Icon, TitlebarStyle_t Style, Component* ParentCpnt) {
        Titlebar_t* Titlebar = (Titlebar_t*)malloc(sizeof(Titlebar_t));
        memcpy(&Titlebar->Style, &Style, sizeof(TitlebarStyle_t));
        Titlebar->MainBox = Box(
            {
                .G{
                    .Width = -100, 
                    .Height = Titlebar_Height, 
                    .Maxwidth = NO_MAXIMUM, 
                    .AutoPosition = true,
                    .IsHidden = false
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
                    .Maxwidth = NO_MAXIMUM, 
                    .Maxheight = NO_MAXIMUM, 
                    .AutoPosition = true,
                    .IsHidden = false
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
                .AutoPosition = true,
                .IsHidden = false
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
                .G{
                    .AutoPosition = false,
                    .IsHidden = false
                },
            }
        , Titlebar->Cpnt);

        Titlebar->BtnBox = Box(
            {
                .G{
                    .Width = Titlebar_Height * 3, 
                    .Height = -100, 
                    .Maxheight = NO_MAXIMUM, 
                    .IsHidden = false
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
                .G{
                    .Width = Titlebar_Height, 
                    .Height = Titlebar_Height, 
                    .AutoPosition = true,
                    .Align = RIGHT,
                    .BorderRadius = Titlebar_Height,
                    .IsHidden = false,
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
                    .Maxwidth = NO_MAXIMUM, 
                    .Maxheight = NO_MAXIMUM, 
                    .IsHidden = false
                }
            }
            , Titlebar->CloseBtn->Cpnt);

        Titlebar->SizeBtn = Button(
            {
                .BackgroundColor = Titlebar->Style.BackgroundColor, 
                .OnMouseEvent = SizeBtnEvent,
                .G{
                    .Width = Titlebar_Height, 
                    .Height = Titlebar_Height, 
                    .AutoPosition = true,
                    .Align = RIGHT,
                    .BorderRadius = Titlebar_Height,
                    .IsHidden = false
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
                    .Maxwidth = NO_MAXIMUM, 
                    .Maxheight = NO_MAXIMUM, 
                    .IsHidden = false
                }
            }
            , Titlebar->SizeBtn->Cpnt);

        Titlebar->HideBtn = Button(
            {
                .BackgroundColor = Titlebar->Style.BackgroundColor, 
                .OnMouseEvent = HideBtnEvent,
                .G{
                    .Width = Titlebar_Height,
                    .Height = Titlebar_Height,
                    .AutoPosition = true,
                    .Align = RIGHT,
                    .BorderRadius = Titlebar_Height,
                    .IsHidden = false
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
                    .Maxwidth = NO_MAXIMUM, 
                    .Maxheight = NO_MAXIMUM, 
                    .IsHidden = false
                }
            }
            , Titlebar->HideBtn->Cpnt);
        return Titlebar;
    }

}