#include <kot-ui++/component.h>

#define Titlebar_Height 25
#define Titlebar_FontSize 16

namespace Ui {
    Titlebar_t* Titlebar(char* Title, char* Icon, TitlebarStyle_t Style, Component* ParentCpnt) {
        Titlebar_t* Titlebar = (Titlebar_t*)malloc(sizeof(Titlebar_t));
        memcpy(&Titlebar->Style, &Style, sizeof(TitlebarStyle_t));
        Titlebar->MainBox = Box(
            {
                .G = { 
                        .Width = -100,
                        .Height = Titlebar_Height,
                        .IsVisible = true
                    },
                .BackgroundColor = Titlebar->Style.BackgroundColor, 
                .ClickColor = Titlebar->Style.BackgroundColor, 
                .HoverColor = Titlebar->Style.BackgroundColor
            }
        , ParentCpnt);
        
        Titlebar->Cpnt = Titlebar->MainBox->Cpnt;

        Titlebar->Logo = Picturebox(Icon, _TGA, 
        {
            .Fit = PICTUREFILL, 
            .G = {
                    .Width = Titlebar_Height, 
                    .Height = Titlebar_Height, 
                    .AutoPosition = true,
                    .IsVisible = true
                }
        }
        , Titlebar->Cpnt);

        Titlebar->Title = Label(
            {   
                .FontPath = "default-font.sfn",
                .Text = Title,
                .FontSize = Titlebar_FontSize,
                .G = { 
                        .Margin = { .Left = 10, .Top = (Titlebar_Height - Titlebar_FontSize) / 2 },
                        .Width = -100, 
                        .Height = Titlebar_Height,
                        .AutoPosition = true,
                        .IsVisible = true
                    },
                .ForegroundColor = Titlebar->Style.ForegroundColor, 
            }
        , Titlebar->Cpnt);

        Titlebar->CloseBtn = Button(
            {
                .G = {  
                        .Width = Titlebar_Height, 
                        .Height = Titlebar_Height, 
                        .Position = {
                            .x = (int64_t)(Titlebar->Cpnt->Style->Currentwidth - Titlebar_Height), 
                            .y = 0
                        },
                        .IsVisible = true
                    },
                .BackgroundColor = Titlebar->Style.BackgroundColor, 
            }
        , Titlebar->Cpnt);

        Picturebox_t* CloseImage = Picturebox("close.tga", _TGA, 
            {
                .Fit = PICTUREFILL, 
                .G = { 
                        .Width = -100, 
                        .Height = -100, 
                        .IsVisible = true
                }
            }
        , Titlebar->CloseBtn->Cpnt);

        Titlebar->SizeBtn = Button(
            {
                .BackgroundColor = Titlebar->Style.BackgroundColor, 
                .G = {
                        .Width = Titlebar_Height, 
                        .Height = Titlebar_Height, 
                        .Position{
                            .x = (int64_t)(Titlebar->CloseBtn->Cpnt->Style->Position.x - Titlebar->CloseBtn->Cpnt->Style->Currentwidth), 
                            .y = 0
                        }, 
                        .IsVisible = true
                    }
            }
        , Titlebar->Cpnt);

        Picturebox_t* SizeImage = Picturebox("maximize.tga", _TGA, 
            {
                .Fit = PICTUREFILL, 
                .G = {
                        .Width = -100, 
                        .Height = -100, 
                        .IsVisible = true
                    }
            }
        , Titlebar->SizeBtn->Cpnt);

        Titlebar->HideBtn = Button(
            {
                .BackgroundColor = Titlebar->Style.BackgroundColor, 
                .G = {
                        .Width = Titlebar_Height, 
                        .Height = Titlebar_Height, 
                        .Position{
                            .x = (int64_t)(Titlebar->SizeBtn->Cpnt->Style->Position.x - Titlebar->SizeBtn->Cpnt->Style->Currentwidth), 
                            .y = 0
                        }, 
                        .IsVisible = true
                    }
            }
        , Titlebar->Cpnt);

        Picturebox_t* HideImage = Picturebox("minimize.tga", _TGA, 
            {
                .Fit = PICTUREFILL, 
                .G = {
                        .Width = -100, 
                        .Height = -100, 
                        .IsVisible = true
                    }
            }
        , Titlebar->HideBtn->Cpnt);

        return Titlebar;
    }

}