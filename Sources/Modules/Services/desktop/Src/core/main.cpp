#include <core/main.h>

#include <kot++/printf.h>

#include <kot/assert.h>

#include <kot-ui++/pictures/picture.h>
using namespace Ui;

void desktopc::SetWallpaper(char *Path)
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

    uint8_t Btpp = Wallpaper->Bpp / 8;
    uint32_t Pitch = Wallpaper->Width * Btpp;

    uintptr_t ImageDataOffset = (uintptr_t)(Wallpaper->ColorMapOrigin + Wallpaper->ColorMapLength + 18),
              ImagePixelData = (uintptr_t)((uint64_t)Wallpaper + (uint64_t)ImageDataOffset);

    uint32_t* Pixels = (uint32_t*) malloc(Fb->Height * (Fb->Width * Fb->Btpp));
    
    uint32_t YPos, XPos;
    std::printf("%x", Wallpaper->Width / Fb->Width);
    for(uint16_t y = 0; y < Fb->Height; y++)
    {   
        YPos = y * Wallpaper->Height / Fb->Height;

        for(uint16_t x = 0; x < Fb->Width; x++)
        {
            XPos = x * Wallpaper->Width / Fb->Width;

            uint64_t index = (uint64_t)ImagePixelData + XPos * Btpp + YPos * Pitch;
            uint8_t B = *(uint8_t*)(index + 0);
            uint8_t G = *(uint8_t*)(index + 1);
            uint8_t R = *(uint8_t*)(index + 2);

            uint8_t A = 0xFF;
            if (Wallpaper->Bpp == 32)
                A = *(uint8_t*)(index + 3);

            PutPixel(Fb, x, y, B | (G << 8) | (R << 16) | (A << 24));
            //Pixels[XPos + YPos*Fb->Width] = B | (G << 8) | (R << 16) | (A << 24);
        }
    }
    std::printf("%x %x %x", *(uint8_t*)((uint64_t)Wallpaper + 0x123CC0), *(uint8_t*)((uint64_t)Wallpaper + 0x123CC1), *(uint8_t*)((uint64_t)Wallpaper + 0x123CC2));

    std::printf("size (height*pitch): %d, array: %d", Fb->Height * (Fb->Width * Fb->Btpp), XPos + YPos*Fb->Width);

    // uint32_t* Pixels = TGARead(Wallpaper, Fb->Width, Fb->Height);

    // for(uint16_t y = 0; y < Fb->Height; y++) {
    //     for(uint16_t x = 0; x < Fb->Width; x++) {
    //         PutPixel(Fb, x, y, Pixels[x + y*Fb->Width]);
    //     }
    // }

    free(Wallpaper);
    free(Pixels);
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
    uint32_t SolidColor = strtol(((JsonString*)Background->Get("solidColor"))->Get(), NULL, 16);

    if(IsWallpaper)
    {
        SetWallpaper(WallpaperPath);
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