#include <kot-ui++/component.h>

#define Titlebar_Height 40
#define Titlebar_FontSize 16

namespace Ui {
    Titlebar_t* Titlebar(char* Title, TitlebarStyle_t Style, Component* ParentCpnt) {
        Titlebar_t* Titlebar = (Titlebar_t*)malloc(sizeof(Titlebar_t));
        memcpy(&Titlebar->Style, &Style, sizeof(TitlebarStyle_t));
        Titlebar->MainBox = Box({.G.Width.Normal = -100, .G.Height.Normal = Titlebar_Height, .BackgroundColor = Titlebar->Style.BackgroundColor, .ClickColor = Titlebar->Style.BackgroundColor, .HoverColor = Titlebar->Style.BackgroundColor, .G.IsVisible = true}, ParentCpnt);
        Titlebar->Cpnt = Titlebar->MainBox->Cpnt;
        // Titlebar->Title = Label({.FontSize = Titlebar_FontSize, .Text  = Title, .FontPath = "default-font.sfn", .G.Width.Normal = Titlebar->Cpnt->Style->Width, .G.Height.Normal = Titlebar->Cpnt->Style->Height, .ForegroundColor = Titlebar->Style.ForegroundColor, .G.IsVisible = true}, ParentUiContex);
        // Titlebar->Cpnt->AddChild(Titlebar->Title->Cpnt);
        // Titlebar->CloseBtn = Button({.BackgroundColor = Titlebar->Style.BackgroundColor, .G.Width.Normal = Titlebar_Height, .G.Height.Normal = Titlebar_Height, .Position{.x = (int64_t)(Titlebar->Cpnt->Style->Width - Titlebar_Height), .y = 0}, .G.IsVisible = true}, ParentUiContex);
        // Titlebar->Cpnt->AddChild(Titlebar->CloseBtn->Cpnt);
        // Picturebox_t* CloseImage = Picturebox("kotlogo.tga", _TGA, {.Fit = PICTUREFILL, .G.Width.Normal = Titlebar_Height, .G.Height.Normal = Titlebar_Height, .IsVisible = true}, ParentUiContex);
        // Titlebar->CloseBtn->Cpnt->AddChild(CloseImage->Cpnt);
        // Titlebar->SizeBtn = Button({.BackgroundColor = Titlebar->Style.BackgroundColor, .G.Width.Normal = Titlebar_Height, .G.Height.Normal = Titlebar_Height, .Position{.x = (int64_t)(Titlebar->CloseBtn->Cpnt->Style->Position.x - Titlebar->CloseBtn->Cpnt->Style->Width.Current), .y = 0}, .G.IsVisible = true}, ParentUiContex);
        // Titlebar->Cpnt->AddChild(Titlebar->SizeBtn->Cpnt);
        // Titlebar->HideBtn = Button({.BackgroundColor = Titlebar->Style.BackgroundColor, .G.Width.Normal = Titlebar_Height, .G.Height.Normal = Titlebar_Height, .Position{.x = (int64_t)(Titlebar->SizeBtn->Cpnt->Style->Position.x - Titlebar->SizeBtn->Cpnt->Style->Width.Current), .y = 0}, .G.IsVisible = true}, ParentUiContex);
        // Titlebar->Cpnt->AddChild(Titlebar->HideBtn->Cpnt);
        return Titlebar;
    }

}