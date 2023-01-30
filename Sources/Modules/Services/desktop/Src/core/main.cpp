#include <core/main.h>

#include <kot++/printf.h>

typedef struct {
    uint8_t idLength;
    uint8_t colorMapType;
    uint8_t imageType;
    uint16_t colorMapOrigin, colorMapLength;
    uint8_t colorMapEntSz;
    uint16_t x, y;
    uint16_t Width, Height;
    uint8_t Bpp;
    uint8_t imageDescriptor;
} __attribute__((__packed__)) tgaHeader_t;

void desktopc::SetWallpaper(char* path) {
    auto wallpaper = Ui::Picturebox(path, Ui::ImageType::_TGA, {});

    file_t* imageFile = fopen(path, "rb");

    fseek(imageFile, 0, SEEK_END);
    size_t imageFileSize = ftell(imageFile);
    fseek(imageFile, 0, SEEK_SET);
    tgaHeader_t* image = (tgaHeader_t*) malloc(imageFileSize);
    fread(image, imageFileSize, 1, imageFile);

    if(wallpaper == NULL) {
        SetSolidColor(NULL);
        return;
    }
}

void desktopc::SetSolidColor(uint32_t color) {
    FillRect(fb, 0, 0, Window_Max_Size, Window_Max_Size, color);
}

desktopc::desktopc(JsonArray* settings) {
    window_t* Desktop = CreateWindow(NULL, Window_Type_Background);
    ResizeWindow(Desktop, Window_Max_Size, Window_Max_Size);

    fb = &Desktop->Framebuffer;

    JsonObject* desktopSettings = (JsonObject*) settings->Get(0);

    /* desktop background */
    JsonObject* background = (JsonObject*) desktopSettings->Get("background");
    bool isWallpaper = ((JsonBoolean*) background->Get("isWallpaper"))->Get();
    char* wallpaperPath = ((JsonString*) background->Get("wallpaperPath"))->Get();
    uint32_t solidColor = strtol(((JsonString*) background->Get("solidColor"))->Get(), NULL, 16);

    if(isWallpaper) {
        SetWallpaper(wallpaperPath);
    } else {
        SetSolidColor(solidColor);
    }

    ChangeVisibilityWindow(Desktop, true);
}

extern "C" int main(int argc, char* argv[]){
    Printlog("[DESKTOP] Initializing...");

    // load desktopUserSettings.json
    
    file_t* settingsFile = fopen("d1:Usr/root/Share/Settings/desktop.json", "r");
    
    if(settingsFile == NULL) {
        Printlog("[DESKTOP] \033[0;31mERR:\033[0m File not found.");
        return KFAIL;
    }

    fseek(settingsFile, 0, SEEK_END);
    size_t settingsFileSize = ftell(settingsFile);
	fseek(settingsFile, 0, SEEK_SET);

    char* BufferSettingsFile = (char*) malloc(settingsFileSize);
    fread(BufferSettingsFile, settingsFileSize, 1, settingsFile);
        
    JsonParser* parser = new JsonParser(BufferSettingsFile);

    if(parser->getCode() != JSON_SUCCESS && parser->getValue()->getType() != JSON_ARRAY) {
        Printlog("[DESKTOP] \033[0;31mERR:\033[0m Json file could not be retrieved.");
        return KFAIL;
    }

    JsonArray* settings = (JsonArray*) parser->getValue();
    desktopc* desktop0 = new desktopc(settings);  

    fclose(settingsFile);

    Printlog("[DESKTOP] Initialized");
    return KSUCCESS;
}