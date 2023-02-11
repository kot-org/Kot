#include <core/main.h>

#include <kot-ui++/pictures/picture.h>
using namespace Ui;

void desktopc::SetWallpaper(char *Path, uint8_t Fit)
{
    file_t* WallpaperFile = fopen(Path, "rb");

    if (WallpaperFile == NULL)
    {
        SetSolidColor(NULL);
        return;
    }

    fseek(WallpaperFile, 0, SEEK_END);
    size_t WallpaperFileSize = ftell(WallpaperFile);
    fseek(WallpaperFile, 0, SEEK_SET);

    TGAHeader_t* Wallpaper = (TGAHeader_t*) malloc(WallpaperFileSize);
    fread(Wallpaper, WallpaperFileSize, 1, WallpaperFile);

    if(Wallpaper->Width <= 0 || Wallpaper->Height <= 0)
    {
        free(Wallpaper);
        fclose(WallpaperFile);
        SetSolidColor(NULL);
        return;
    }

    TGA_t* Image = TGARead(Wallpaper);

    switch(Fit)
    {
        case WallpaperFit::FIT:
        {
            // todo: resize l'image jusqu'a ce qu'elle touche le bord (horizontalement ou verticalement)

            TGA_t* ImageResize = TGAResize(Image, Fb->Width, Fb->Height);
            TGADraw(Fb, ImageResize);

            free(ImageResize);
            break;
        }

        case WallpaperFit::FILL:
        {
            break;
        }

        case WallpaperFit::CENTER:
        {
            break;
        }

        default:
            SetSolidColor(NULL);
            break;
    }

    free(Image);

    free(Wallpaper);
    fclose(WallpaperFile);
}

void desktopc::SetSolidColor(uint32_t Color)
{
    FillRect(Fb, 0, 0, Fb->Width, Fb->Height, Color);
}

desktopc::desktopc(JsonArray* Settings)
{
    window_t* Desktop = CreateWindow(NULL, Window_Type_Background);
    ResizeWindow(Desktop, Window_Max_Size, Window_Max_Size);

    Fb = &Desktop->Framebuffer;

    JsonObject* DesktopSettings = (JsonObject*)Settings->Get(0);

    /* desktop background */
    JsonObject* Background = (JsonObject*)DesktopSettings->Get("background");
    bool IsWallpaper = ((JsonBoolean*)Background->Get("isWallpaper"))->Get();
    char* WallpaperPath = ((JsonString*)Background->Get("wallpaperPath"))->Get();
    uint8_t WallpaperFit = ((JsonNumber*)Background->Get("wallpaperFit"))->Get();
    uint32_t SolidColor = strtol(((JsonString*)Background->Get("solidColor"))->Get(), NULL, 16);

    if(IsWallpaper)
    {
        SetWallpaper(WallpaperPath, WallpaperFit);
    } else
    {
        SetSolidColor(SolidColor);
    }

    // todo: creer une grille pour les icones

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