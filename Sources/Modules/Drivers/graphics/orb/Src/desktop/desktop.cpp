#include <desktop/desktop.h>

void EventIcon(struct Button_t* Button, ButtonStatus_t EventType){
    return;
}

desktopc::desktopc(orbc* Parent){
    Printlog("[ORB/DESKTOP] Initializing...");
    
    /* Play startup sound */
    Audio::Stream* St = new Audio::Stream(0);

    file_t* MusicFile = fopen("d1:Usr/root/Share/Sounds/startup.bin", "r");

    fseek(MusicFile, 0, SEEK_END);
    size64_t FileSize = ftell(MusicFile);
    fseek(MusicFile, 0, SEEK_SET); 

    uintptr_t FileBuf = malloc(FileSize);
    fread(FileBuf, FileSize, 1, MusicFile);

    St->AddBuffer(FileBuf, FileSize);

    /* Load desktopUserSettings.json */

    file_t* SettingsFile = fopen("d1:Usr/root/Share/Settings/desktop.json", "r");

    assert(SettingsFile != NULL);

    fseek(SettingsFile, 0, SEEK_END);
    size_t SettingsFileSize = ftell(SettingsFile);
    fseek(SettingsFile, 0, SEEK_SET);

    char* BufferSettingsFile = (char*)malloc(SettingsFileSize);
    fread(BufferSettingsFile, SettingsFileSize, 1, SettingsFile);

    JsonParser* parser = new JsonParser(BufferSettingsFile);

    assert(parser->getCode() == JSON_SUCCESS);
    assert(parser->getValue()->getType() == JSON_ARRAY);

    JsonArray* Settings = (JsonArray*)parser->getValue();
    assert(Settings != NULL);

    JsonObject* DefaultSetting = (JsonObject*)Settings->Get(0);
    assert(DefaultSetting != NULL);

    JsonObject* DesktopSettings = (JsonObject*)DefaultSetting->Get("desktop");
    assert(DesktopSettings != NULL);


    /* desktop background */
    JsonObject* Background = (JsonObject*)DesktopSettings->Get("background");
    IsWallpaper = ((JsonBoolean*)Background->Get("isWallpaper"))->Get();
    WallpaperPath = ((JsonString*)Background->Get("wallpaperPath"))->Get();
    WallpaperFit = ((JsonNumber*)Background->Get("wallpaperFit"))->Get();
    SolidColor = strtol(((JsonString*)Background->Get("solidColor"))->Get(), NULL, 16);


    /* clock */
    IsClock = ((JsonBoolean*)Background->Get("isClock"))->Get();
    FontPathClock = ((JsonString*)Background->Get("clockFontPath"))->Get();


    /* applications */
    // todo: creer une grille pour les icones


    JsonObject* TaskbarSettings = (JsonObject*)DefaultSetting->Get("taskbar");
    assert(TaskbarSettings != NULL);
    UpdateTaskbar(TaskbarSettings);

    fclose(SettingsFile);

    Printlog("[ORB/DESKTOP] Initialized");
}

KResult desktopc::AddMonitor(monitorc* Monitor){
    Monitor->DesktopData = (desktopmonitor*)malloc(sizeof(desktopmonitor));

    /* Desktop */
    Monitor->DesktopData->FbDesktop = (framebuffer_t*)malloc(sizeof(framebuffer_t));
    Monitor->DesktopData->FbDesktop->Buffer = calloc(Monitor->BackFramebuffer->Size);
    Monitor->DesktopData->FbDesktop->Width = Monitor->BackFramebuffer->Width;
    Monitor->DesktopData->FbDesktop->Height = Monitor->BackFramebuffer->Height;
    Monitor->DesktopData->FbDesktop->Pitch = Monitor->BackFramebuffer->Pitch;
    Monitor->DesktopData->FbDesktop->Bpp = Monitor->BackFramebuffer->Bpp;
    Monitor->DesktopData->FbDesktop->Btpp = Monitor->BackFramebuffer->Btpp;
    Monitor->DesktopData->FbDesktop->Size = Monitor->BackFramebuffer->Size;
    Monitor->DesktopData->UiCtxDesktop = new Ui::UiContext(Monitor->DesktopData->FbDesktop);
    Monitor->DesktopData->Parent = this;
    
    // TODO : LoadBootGraphics(Monitor->DesktopData->Fb);

    Monitor->Orb->Desktop = this;

    if(IsWallpaper){
        Monitor->DesktopData->SetWallpaper(WallpaperPath, (PictureboxFit)WallpaperFit);
    }else{
        Monitor->DesktopData->SetSolidColor(SolidColor);
    }
    if(IsClock){
        Monitor->DesktopData->InitalizeClock(FontPathClock);
    }

    Monitor->DesktopData->UiCtxDesktop->UiStartRenderer();

    /* Taskbar */
    Monitor->DesktopData->FbTaskbar = (framebuffer_t*)malloc(sizeof(framebuffer_t));
    Monitor->DesktopData->FbTaskbar->Buffer = calloc(Taskbar->Height * Monitor->BackFramebuffer->Pitch);
    Monitor->DesktopData->FbTaskbar->Width = Monitor->BackFramebuffer->Width;
    Monitor->DesktopData->FbTaskbar->Height = Taskbar->Height;
    Monitor->DesktopData->FbTaskbar->Pitch = Monitor->BackFramebuffer->Pitch;
    Monitor->DesktopData->FbTaskbar->Bpp = Monitor->BackFramebuffer->Bpp;
    Monitor->DesktopData->FbTaskbar->Btpp = Monitor->BackFramebuffer->Btpp;
    Monitor->DesktopData->FbTaskbar->Size = Taskbar->Height * Monitor->BackFramebuffer->Pitch;
    Monitor->DesktopData->UiCtxTaskbar = new UiContext(Monitor->DesktopData->FbTaskbar);

    Monitor->DesktopData->TaskBarPositon.x = 0;
    Monitor->DesktopData->TaskBarPositon.y = Monitor->DesktopData->FbDesktop->Height - Taskbar->Height;

    Box_t* FlexContainer = Box(
        {
            .G = {
                    .Width = -100,
                    .Height = -100,
                },
            .BackgroundColor = Taskbar->SolidColor
        }
        , Monitor->DesktopData->UiCtxTaskbar->Cpnt);

    Ui::Flexbox_t* AppsContainer = Ui::Flexbox(
        {
            .Direction = Layout::ROW,
            .G = { 
                    .Width = -100,
                    .Height = -100,
                    .Position{
                        .x = 0,
                        .y = 0
                    },
                    .AutoPosition = false, 
                }, 
            .Align = { 
                .x = Layout::CENTER, 
                .y = Layout::MIDDLE 
            }
        }
    , FlexContainer->Cpnt);
    
    for(uint8_t i = 0; i < Taskbar->Applications->length(); i++) {
        JsonObject* App = (JsonObject*)Taskbar->Applications->Get(i);

        uint8_t AppPosition = ((JsonNumber*) App->Get("position"))->Get();

        Button_t* AppButton = Button(EventIcon,
        {
            .G = {
                    .Width = Taskbar->IconSize,
                    .Height = Taskbar->IconSize,
                    .Position = { .x = AppPosition * Taskbar->IconSize },
                    .Margin.Left = 20,
                    .IsHidden = false
                },
            .BackgroundColor = 0xFF0000,
            .HoverColor = 0x00FF00
        }
        , AppsContainer->Cpnt);

        Picturebox_t* AppIcon = Picturebox("kotlogo.tga", PictureboxType::_TGA,
            {
                .Fit = Ui::PICTUREFILL,
                .Transparency = true,
                .G{
                    .Width = -100, 
                    .Height = -100, 
                    .IsHidden = false
                }
            }
        , AppButton->Cpnt);
    }

    Monitor->DesktopData->UiCtxTaskbar->UiStartRenderer();

    return KSUCCESS;
}

KResult desktopc::RemoveMonitor(monitorc* Monitor){
    Monitor->Orb->Desktop = this;
    return KSUCCESS;
}

KResult desktopc::UpdateBackground(monitorc* Monitor){
    memcpy(Monitor->BackFramebuffer->Buffer, Monitor->DesktopData->FbDesktop->Buffer, Monitor->DesktopData->FbDesktop->Size);
    return KSUCCESS;
}

KResult desktopc::UpdateWidgets(monitorc* Monitor){
    // Taskbar
    BlitFramebuffer(Monitor->BackFramebuffer, Monitor->DesktopData->FbTaskbar, Monitor->DesktopData->TaskBarPositon.x, Monitor->DesktopData->TaskBarPositon.y);
    return KSUCCESS;
}

// Private functions

void desktopmonitor::SetWallpaper(char* Path, PictureboxFit Fit) {
    Wallpaper = Ui::Picturebox(Path, _TGA, 
        {
            .Fit = Fit,
            .G = {
                    .Width = -100,
                    .Maxwidth = NO_MAXIMUM,
                    .Height = -100,
                    .Maxheight = NO_MAXIMUM,
                    .Position = {
                        .x = 0,
                        .y = 0
                    },
                    .IsHidden = false
                }
        }
    , UiCtxDesktop->Cpnt);
}

void desktopmonitor::SetSolidColor(uint32_t Color){
    FillRect(FbDesktop, 0, 0, FbDesktop->Width, FbDesktop->Height, Color);
}

char* WeekdayString[] = {
    "sunday",
    "monday",
    "tuesday",
    "wednesday",
    "thurday",
    "friday",
    "saturday",
};

char* MonthString[] = {
    "january",
    "february",
    "march",
    "april",
    "may",
    "june",
    "july",
    "august",
    "september",
    "october",
    "november",
    "december",
};

void UpdateClock(Ui::Label_t* Time, Ui::Label_t* Date){
    char TimeStr[1024];
    char DateStr[1024];
    uint64_t TimerState;

    if(GetSecond()){
        Sleep((60 - GetSecond()) * 1000);
    }
    GetActualTick(&TimerState);

    while(true){
        sprintf((char*)&TimeStr, "%d:%d", GetHour(), GetMinute());
        Time->UpdateText((char*)&TimeStr);
        sprintf((char*)&DateStr, "%d %s, %d", GetDay(), MonthString[GetMonth()-1], 2023);
        Date->UpdateText((char*)&DateStr);
        SleepFromTick(&TimerState, 60000);
    }
    
    Sys_Close(KSUCCESS);
}

void desktopmonitor::InitalizeClock(char* FontPath){
    Ui::Flexbox_t* ClockContainer = Ui::Flexbox(
        {
            .Direction = Layout::COLUMN,
            .G = { 
                    .Width = -100,
                    .Height = -100,
                    .Position{
                        .x = 0,
                        .y = 0
                    },
                    .AutoPosition = false, 
                }, 
            .Align = { 
                .x = Layout::FILLHORIZONTAL, 
                .y = Layout::TOP 
            }
        }
    , UiCtxDesktop->Cpnt);

    char TimeStr[1024];
    sprintf((char*)&TimeStr, "%d:%d", GetHour(), GetMinute());

    Ui::Label_t* Time = Ui::Label(
        {
            .Text = TimeStr,
            .FontPath = FontPath,
            .FontSize = 72,
            .ForegroundColor = 0xffffffff,
            .Align = Ui::TEXTALIGNCENTER,
            .AutoWidth = false,
            .AutoHeight = true,
            .G = {
                    .Margin = {
                        .Top = 20,
                    },
                    .Width = -100,
                    .AutoPosition = false,        
                }
        }
    , ClockContainer->Cpnt);

    char DateStr[1024];
    sprintf((char*)&DateStr, "%d %s, %d", GetDay(), MonthString[GetMonth()-1], 2023);

    Ui::Label_t* Date = Ui::Label(
        {
            .Text = DateStr,
            .FontPath = FontPath,
            .FontSize = 16,
            .ForegroundColor = 0xffffffff,
            .Align = Ui::TEXTALIGNCENTER,
            .AutoWidth = false,
            .AutoHeight = true,
            .G = {
                    .Margin = {
                        .Top = 20,
                    },
                    .Width = -100, 
                    .AutoPosition = false,          
                }
        }
    , ClockContainer->Cpnt);

    Sys_CreateThread(Sys_GetProcess(), (uintptr_t)&UpdateClock, PriviledgeApp, NULL, &ClockThread);
    
    arguments_t Parameters{
        .arg[0] = (uint64_t)Time,
        .arg[1] = (uint64_t)Date,
    };
    Sys_ExecThread(ClockThread, &Parameters, ExecutionTypeQueu, NULL);
}

void desktopc::UpdateTaskbar(JsonObject* TaskbarSettings) {
    Taskbar = (Taskbar_t*)malloc(sizeof(Taskbar_t));

    JsonObject* Style = (JsonObject*)TaskbarSettings->Get("style");
    assert(Style != NULL);
    Taskbar->Height = ((JsonNumber*) Style->Get("height"))->Get();
    Taskbar->IsExtended = ((JsonBoolean*) Style->Get("isExtended"))->Get();
    Taskbar->IconSize = ((JsonNumber*) Style->Get("iconSize"))->Get();
    Taskbar->SolidColor = strtol(((JsonString*)Style->Get("solidColor"))->Get(), NULL, 16);
    Taskbar->Applications = (JsonArray*)TaskbarSettings->Get("applications");
}