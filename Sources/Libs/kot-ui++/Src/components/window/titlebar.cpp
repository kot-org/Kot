#include <kot-ui++/component.h>

#define Titlebar_Height 25
#define Titlebar_FontSize 16

namespace Ui {
    Titlebar_t* Titlebar(char* Title, char* Icon, TitlebarStyle_t Style, Component* ParentCpnt) {
        Titlebar_t* Titlebar = (Titlebar_t*)malloc(sizeof(Titlebar_t));
        memcpy(&Titlebar->Style, &Style, sizeof(TitlebarStyle_t));
        Titlebar->MainBox = Box(
            {
            .G.Width = -100, 
            .G.Height = Titlebar_Height, 
            .G.Maxwidth = NO_MAXIMUM, 
            .BackgroundColor = Titlebar->Style.BackgroundColor, 
            .ClickColor = Titlebar->Style.BackgroundColor, 
            .HoverColor = Titlebar->Style.BackgroundColor, 
            .G.AutoPosition = true,
            .G.IsHidden = false
            }
        , ParentCpnt);
        
        Titlebar->Cpnt = Titlebar->MainBox->Cpnt;

        Titlebar->Logo = Picturebox(Icon, _TGA, 
        {
            .Fit = PICTUREFILL, 
            .G.Width = Titlebar_Height, 
            .G.Height = Titlebar_Height, 
            .G.AutoPosition = true,
            .G.IsHidden = false
        }
        , Titlebar->Cpnt);

        Titlebar->Title = Label(
            {   
                .FontPath = "default-font.sfn",
                .Text = Title,
                .FontSize = Titlebar_FontSize,
                .G.Margin{
                    .Left = 10,
                    .Top = (Titlebar_Height - Titlebar_FontSize) / 2
                },
                .G.Width = -100, 
                .G.Maxwidth = NO_MAXIMUM, 
                .G.Height = Titlebar_Height, 
                .ForegroundColor = Titlebar->Style.ForegroundColor, 
                .G.AutoPosition = true,
                .G.IsHidden = false
            }
        , Titlebar->Cpnt);

        Titlebar->CloseBtn = Button(
            {
                .G.Width = Titlebar_Height, 
                .G.Height = Titlebar_Height, 
                .G.AutoPosition = true,
                .G.Align = RIGHT,
                .BackgroundColor = Titlebar->Style.BackgroundColor, 
                .G.IsHidden = false
            }
            , Titlebar->Cpnt);

        Picturebox_t* CloseImage = Picturebox("close.tga", _TGA, 
            {
                .Fit = PICTUREFILL, 
                .G.Width = -100, 
                .G.Height = -100, 
                .G.Maxwidth = NO_MAXIMUM, 
                .G.Maxheight = NO_MAXIMUM, 
                .G.IsHidden = false
            }
            , Titlebar->CloseBtn->Cpnt);

        Titlebar->SizeBtn = Button(
            {
                .BackgroundColor = Titlebar->Style.BackgroundColor, 
                .G.Width = Titlebar_Height, 
                .G.Height = Titlebar_Height, 
                .G.AutoPosition = true,
                .G.Align = RIGHT,
                .G.IsHidden = false
            }
            , Titlebar->Cpnt);

        Picturebox_t* SizeImage = Picturebox("maximize.tga", _TGA, 
            {
                .Fit = PICTUREFILL, 
                .G.Width = -100, 
                .G.Height = -100, 
                .G.Maxwidth = NO_MAXIMUM, 
                .G.Maxheight = NO_MAXIMUM, 
                .G.IsHidden = false
            }
            , Titlebar->SizeBtn->Cpnt);

        Titlebar->HideBtn = Button(
            {
                .BackgroundColor = Titlebar->Style.BackgroundColor, 
                .G.Width = Titlebar_Height,
                .G.Height = Titlebar_Height,
                .G.AutoPosition = true,
                .G.Align = RIGHT,
                .G.IsHidden = false
            }
        , Titlebar->Cpnt);

        Picturebox_t* HideImage = Picturebox("minimize.tga", _TGA, 
            {
                .Fit = PICTUREFILL, 
                .G.Width = -100, 
                .G.Height = -100, 
                .G.Maxwidth = NO_MAXIMUM, 
                .G.Maxheight = NO_MAXIMUM, 
                .G.IsHidden = false
            }
            , Titlebar->HideBtn->Cpnt);
        return Titlebar;
    }

}