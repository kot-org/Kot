#include <core/main.h>

#include <kot-ui++/pictures/picture.h>
using namespace Ui;

#include <kot++/printf.h>

// todo: fonction "AddWallpaper" qui permet d'ajouter un wallpaper dans "USER/Share/Wallpaper" et de modifier les parametres "desktop.json" pour changer le fit en fonction de la taille de l'image (modifiable par l'utilisateur)

// todo: fonction "ChangeFit"

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

    if(Image == NULL) {
        SetSolidColor(NULL);
        return;
    }

    switch(Fit)
    {
        case WallpaperFit::FIT:
        {
            TGA_t* ImageResize = TGAResize(Image, Fb->Width, Fb->Height, true);

            ImageResize->x = Fb->Width / 2 - ImageResize->Width / 2;
            ImageResize->y = Fb->Height / 2 - ImageResize->Height / 2;

            TGADraw(Fb, ImageResize);

            free(ImageResize);
            break;
        }

        case WallpaperFit::FILL:
        {
            uint16_t _Width = Image->Width, _Height = Image->Height;

            // permet de faire en sorte que l'image dépasse la taille du monitor
            if(_Width > _Height) {
                _Width = NULL;
                _Height = Fb->Height;
            } else if(_Height > _Width) {
                _Width = Fb->Width;
                _Height = NULL;
            }
            
            TGA_t* ImageResize = TGAResize(Image, _Width, _Height, true);

            uint16_t x = (ImageResize->Width - Fb->Width) / 2;
            uint16_t y = (ImageResize->Height - Fb->Height) / 2;

            TGA_t* ImageCrop = TGACrop(ImageResize, Fb->Width, Fb->Height, x, y);
            
            TGADraw(Fb, ImageCrop);

            free(ImageResize);
            break;
        }

        case WallpaperFit::CENTER:
        {
            uint16_t x = 0, y = 0,
                _Width = Image->Width, _Height = Image->Height;

            if(_Width > Fb->Width) {
                x = (_Width - Fb->Width) / 2;
                _Width = Fb->Width;
            }
            if(_Height > Fb->Height) {
                y = (_Height - Fb->Height) / 2;
                _Height = Fb->Height;
            }

            // si l'image dépasse le monitor alors on crop
            if(Image->Width > Fb->Width || Image->Height > Fb->Height) {
                TGA_t* ImageCrop = TGACrop(Image, _Width, _Height, x, y);

                ImageCrop->x = Fb->Width / 2 - ImageCrop->Width / 2;
                ImageCrop->y = Fb->Height / 2 - ImageCrop->Height / 2;

                TGADraw(Fb, ImageCrop);
                
                free(ImageCrop);
            } else {
                Image->x = Fb->Width / 2 - Image->Width / 2;
                Image->y = Fb->Height / 2 - Image->Height / 2;
                
                TGADraw(Fb, Image);
            }

            break;
        }

        case WallpaperFit::STRETCH:
        {
            TGA_t* ImageResize = TGAResize(Image, Fb->Width, Fb->Height);
            TGADraw(Fb, ImageResize);

            free(ImageResize);
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