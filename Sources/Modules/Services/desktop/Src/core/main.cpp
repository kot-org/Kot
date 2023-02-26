#include <core/main.h>

#include <kot-ui++/pictures/picture.h>
using namespace Ui;

#include <kot++/printf.h>

// todo: fonction "AddWallpaper" qui permet d'ajouter un wallpaper dans "USER/Share/Wallpaper" et de modifier les parametres "desktop.json" pour changer le fit en fonction de la taille de l'image (modifiable par l'utilisateur)

// todo: fonction "ChangeFit"

void desktopc::InitalizeClock(char* FontPath){
    Ui::Flexbox_t* ClockContainer = Ui::Flexbox({
        .G = { 
                .Width = -100,
                .Maxwidth = NO_MAXIMUM,
                .Height = -100,
                .Maxheight = NO_MAXIMUM,
                .Position{
                    .x = 0,
                    .y = 0
                },
                .IsHidden = false 
            }, 
        .Align = { 
            .x = Layout::FILLHORIZONTAL, 
            .y = Layout::MIDDLE 
        }
    }, UiCtx->Cpnt);

    Ui::Label_t* Weekday = Ui::Label({
        .Text = "Sunday",
        .FontPath = FontPath,
        .FontSize = 72,
        .ForegroundColor = 0xffffffff,
        .Align = Ui::TEXTALIGNCENTER,
        .G{
            .Width = -100,
            .Maxwidth = NO_MAXIMUM,
            .Height = 72,
            .IsHidden = false            
        }
    }, ClockContainer->Cpnt);
}

void desktopc::SetWallpaper(char* Path, Ui::PictureboxFit Fit){
    Wallpaper = Ui::Picturebox(Path, _TGA, {
        .Fit = Fit,
        .G{
            .Width = -100,
            .Maxwidth = NO_MAXIMUM,
            .Height = -100,
            .Maxheight = NO_MAXIMUM,
            .Position{
                .x = 0,
                .y = 0
            },
            .IsHidden = false
        }
    }, UiCtx->Cpnt);
}

void desktopc::SetSolidColor(uint32_t Color){
    FillRect(Fb, 0, 0, Fb->Width, Fb->Height, Color);
}

desktopc::desktopc(JsonArray* Settings){
    window_t* Desktop = CreateWindow(NULL, Window_Type_Background);
    ResizeWindow(Desktop, Window_Max_Size, Window_Max_Size);

    Fb = &Desktop->Framebuffer;

    UiCtx = new Ui::UiContext(Fb);


    JsonObject* DesktopSettings = (JsonObject*)Settings->Get(0);

    /* desktop background */
    JsonObject* Background = (JsonObject*)DesktopSettings->Get("background");
    bool IsWallpaper = ((JsonBoolean*)Background->Get("isWallpaper"))->Get();
    char* WallpaperPath = ((JsonString*)Background->Get("wallpaperPath"))->Get();
    uint8_t WallpaperFit = ((JsonNumber*)Background->Get("wallpaperFit"))->Get();
    uint32_t SolidColor = strtol(((JsonString*)Background->Get("solidColor"))->Get(), NULL, 16);

    if(IsWallpaper)
    {
        SetWallpaper(WallpaperPath, (Ui::PictureboxFit)WallpaperFit);
    } else
    {
        SetSolidColor(SolidColor);
    }

    bool IsClock = ((JsonBoolean*)Background->Get("isClock"))->Get();
    char* FontPathClock = ((JsonString*)Background->Get("clockFontPath"))->Get();

    if(IsClock){
        InitalizeClock(FontPathClock);
    }

    // todo: creer une grille pour les icones

    UiCtx->UiStartRenderer();
    ChangeVisibilityWindow(Desktop, true);
}

extern "C" int main(int argc, char *argv[])
{
    Printlog("[DESKTOP] Initializing...");

    // load desktopUserSettings.json

    file_t* SettingsFile = fopen("d1:Usr/root/Share/Settings/desktop.json", "r");

    if (SettingsFile == NULL)
    {
        Printlog("[DESKTOP] \033[0;31mERR:\033[0m File not found.");
        return KFAIL;
    }

    fseek(SettingsFile, 0, SEEK_END);
    size_t SettingsFileSize = ftell(SettingsFile);
    fseek(SettingsFile, 0, SEEK_SET);

    char* BufferSettingsFile = (char*)malloc(SettingsFileSize);
    fread(BufferSettingsFile, SettingsFileSize, 1, SettingsFile);

    JsonParser* parser = new JsonParser(BufferSettingsFile);

    if (parser->getCode() != JSON_SUCCESS && parser->getValue()->getType() != JSON_ARRAY)
    {
        Printlog("[DESKTOP] \033[0;31mERR:\033[0m Json file could not be retrieved.");
        return KFAIL;
    }

    JsonArray* Settings = (JsonArray*)parser->getValue();
    desktopc* desktop0 = new desktopc(Settings);

    fclose(SettingsFile);

    Printlog("[DESKTOP] Initialized");
    return KSUCCESS;
}