#include <core/main.h>

#include <kot++/printf.h>

desktopc::desktopc(JsonArray* settings) {
    window_t* Window = CreateWindow(NULL, Window_Type_Background);
    ResizeWindow(Window, Window_Max_Size, Window_Max_Size);

    fb = &Window->Framebuffer;

    JsonObject* desktopSettings = (JsonObject*) settings->Get(0);

    /* desktop background */
    JsonObject* background = (JsonObject*) desktopSettings->Get("background");
    bool isWallpaper = ((JsonBoolean*) background->Get("isWallpaper"))->Get();
    char* wallpaperPath = ((JsonString*) background->Get("wallpaperPath"))->Get();
    uint32_t solidColor = strtol(((JsonString*) background->Get("solidColor"))->Get(), NULL, 16);

    if(isWallpaper) {
        if(wallpaperPath != NULL) {
            file_t* wallpaperFile = fopen(wallpaperPath, "r");

            if(wallpaperFile == NULL)
                goto loadSolidColor;

            // todo: afficher le wallpaper (et terminer le tga parser)
        }
    } else {
        loadSolidColor:
        FillRect(fb, 0, 0, Window_Max_Size, Window_Max_Size, solidColor);
    }

    ChangeVisibilityWindow(Window, true);
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
    uint64_t size = ftell(settingsFile);
	fseek(settingsFile, 0, SEEK_SET);

    char* BufferSettingsFile = (char*) malloc(size);
    fread(BufferSettingsFile, size, 1, settingsFile);
        
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