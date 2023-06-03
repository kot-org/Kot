#include <desktop/desktop.h>

#include <kot++/printf.h>

void MouseHandlerDesktop(uint64_t EventType, uint64_t PositionX, uint64_t PositionY, uint64_t ZValue, uint64_t Status){
    monitorc* Monitor = (monitorc*)kot_Sys_GetExternalDataThread();
    Monitor->DesktopData->Desktop->MouseHandler(PositionX, PositionY, ZValue, Status);
    kot_Sys_Event_Close();
}

void MouseHandlerTaskbar(uint64_t EventType, uint64_t PositionX, uint64_t PositionY, uint64_t ZValue, uint64_t Status){
    monitorc* Monitor = (monitorc*)kot_Sys_GetExternalDataThread();
    Monitor->DesktopData->Taskbar->MouseHandler(PositionX, PositionY, ZValue, Status);
    kot_Sys_Event_Close();
}

desktopc::desktopc(orbc* Parent){
    kot_Printlog("[ORB/DESKTOP] Initializing...");
    
    /* Play startup sound */
    Audio::Stream* St = new Audio::Stream(0);

    FILE* MusicFile = fopen("d1:Kot/Sounds/startup.bin", "r");

    fseek(MusicFile, 0, SEEK_END);
    size64_t FileSize = ftell(MusicFile);
    fseek(MusicFile, 0, SEEK_SET); 

    void* FileBuf = malloc(FileSize);
    fread(FileBuf, FileSize, 1, MusicFile);
    fclose(MusicFile);

    St->AddBuffer(FileBuf, FileSize);

    /* Load desktopUserSettings.json */

    FILE* SettingsFile = fopen("d1:User/root/Share/Settings/desktop.json", "r");

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

    kot_Printlog("[ORB/DESKTOP] Initialized");
}

KResult desktopc::AddMonitor(monitorc* Monitor){
    /* Add dock : fixed widgets */
    Monitor->XPositionWithDock = Monitor->XPosition;                                    // left
    Monitor->XMaxPositionWithDock = Monitor->XMaxPosition;                              // right
    Monitor->YPositionWithDock = Monitor->YPosition;                                    // top
    Monitor->YMaxPositionWithDock = Monitor->YMaxPosition - Taskbar->Height;            // bottom

    Monitor->DesktopData = (desktopmonitor*)malloc(sizeof(desktopmonitor));

    /* Desktop */
    Monitor->DesktopData->Desktop = (desktopcomponent*)malloc(sizeof(desktopcomponent));
    Monitor->DesktopData->Desktop->Fb = (kot_framebuffer_t*)malloc(sizeof(kot_framebuffer_t));
    Monitor->DesktopData->Desktop->Fb->Buffer = calloc(1, Monitor->BackFramebuffer->Size);
    Monitor->DesktopData->Desktop->Fb->Width = Monitor->BackFramebuffer->Width;
    Monitor->DesktopData->Desktop->Fb->Height = Monitor->BackFramebuffer->Height;
    Monitor->DesktopData->Desktop->Fb->Pitch = Monitor->BackFramebuffer->Pitch;
    Monitor->DesktopData->Desktop->Fb->Bpp = Monitor->BackFramebuffer->Bpp;
    Monitor->DesktopData->Desktop->Fb->Btpp = Monitor->BackFramebuffer->Btpp;
    Monitor->DesktopData->Desktop->Fb->Size = Monitor->BackFramebuffer->Size;

    Monitor->DesktopData->Desktop->MouseEvent = (hid_event_t*)malloc(sizeof(hid_event_t));
    kot_Sys_Event_Create(&Monitor->DesktopData->Desktop->MouseEvent->Event);
    kot_Sys_CreateThread(kot_Sys_GetProcess(), (void*)&MouseHandlerDesktop, PriviledgeApp, (uint64_t)Monitor, &Monitor->DesktopData->Desktop->MouseEventThread);
    kot_Sys_Event_Bind(Monitor->DesktopData->Desktop->MouseEvent->Event, Monitor->DesktopData->Desktop->MouseEventThread, true);

    Monitor->DesktopData->Desktop->MouseEvent->ParentType = MOUSE_EVENT_PARENT_TYPE_WIDGET;
    Monitor->DesktopData->Desktop->MouseEvent->Parent = Monitor;

    Monitor->DesktopData->Desktop->Eventbuffer = CreateEventBuffer(Monitor->DesktopData->Desktop->Fb->Width, Monitor->DesktopData->Desktop->Fb->Height);
    kot_memset64(Monitor->DesktopData->Desktop->Eventbuffer->Buffer, (uint64_t)Monitor->DesktopData->Desktop->MouseEvent, Monitor->DesktopData->Desktop->Eventbuffer->Size);
    
    Monitor->DesktopData->Desktop->UiCtx = new Ui::UiContext(Monitor->DesktopData->Desktop->Fb);

    Monitor->DesktopData->Desktop->Position.x = Monitor->XPosition;
    Monitor->DesktopData->Desktop->Position.y = Monitor->YPosition;

    Monitor->DesktopData->Parent = this;
    
    // TODO : LoadBootGraphics(Monitor->DesktopData->Fb);

    Monitor->Orb->Desktop = this;

    if(IsWallpaper)
        Monitor->DesktopData->SetWallpaper(WallpaperPath, (PictureboxFit)WallpaperFit);
    else
        Monitor->DesktopData->SetSolidColor(SolidColor);

    if(IsClock)
        Monitor->DesktopData->InitalizeClock(FontPathClock);

    Monitor->DesktopData->Desktop->UiCtx->UiStartRenderer();

    /* Taskbar */
    Monitor->DesktopData->Taskbar = (desktopcomponent*)malloc(sizeof(desktopcomponent));
    Monitor->DesktopData->Taskbar->Fb = (kot_framebuffer_t*)malloc(sizeof(kot_framebuffer_t));
    Monitor->DesktopData->Taskbar->Fb->Buffer = calloc(Taskbar->Height, Monitor->BackFramebuffer->Pitch);
    Monitor->DesktopData->Taskbar->Fb->Width = Monitor->BackFramebuffer->Width;
    Monitor->DesktopData->Taskbar->Fb->Height = Taskbar->Height;
    Monitor->DesktopData->Taskbar->Fb->Pitch = Monitor->BackFramebuffer->Pitch;
    Monitor->DesktopData->Taskbar->Fb->Bpp = Monitor->BackFramebuffer->Bpp;
    Monitor->DesktopData->Taskbar->Fb->Btpp = Monitor->BackFramebuffer->Btpp;
    Monitor->DesktopData->Taskbar->Fb->Size = Taskbar->Height * Monitor->BackFramebuffer->Pitch;

    Monitor->DesktopData->Taskbar->MouseEvent = (hid_event_t*)malloc(sizeof(hid_event_t));
    kot_Sys_Event_Create(&Monitor->DesktopData->Taskbar->MouseEvent->Event);
    kot_Sys_CreateThread(kot_Sys_GetProcess(), (void*)&MouseHandlerTaskbar, PriviledgeApp, (uint64_t)Monitor, &Monitor->DesktopData->Taskbar->MouseEventThread);
    kot_Sys_Event_Bind(Monitor->DesktopData->Taskbar->MouseEvent->Event, Monitor->DesktopData->Taskbar->MouseEventThread, true);

    Monitor->DesktopData->Taskbar->MouseEvent->ParentType = MOUSE_EVENT_PARENT_TYPE_WIDGET;
    Monitor->DesktopData->Taskbar->MouseEvent->Parent = Monitor;

    Monitor->DesktopData->Taskbar->Eventbuffer = CreateEventBuffer(Monitor->DesktopData->Taskbar->Fb->Width, Monitor->DesktopData->Taskbar->Fb->Height);
    kot_memset64(Monitor->DesktopData->Taskbar->Eventbuffer->Buffer, (uint64_t)Monitor->DesktopData->Taskbar->MouseEvent, Monitor->DesktopData->Taskbar->Eventbuffer->Size);

    Monitor->DesktopData->Taskbar->UiCtx = new UiContext(Monitor->DesktopData->Taskbar->Fb);

    Monitor->DesktopData->Taskbar->Position.x = Monitor->XPosition;
    Monitor->DesktopData->Taskbar->Position.y = Monitor->DesktopData->Desktop->Fb->Height - Taskbar->Height + Monitor->YPosition;

    Box_t* FlexContainer = Box(
        {
            .G = {
                    .Width = -100,
                    .Height = -100,
                },
            .BackgroundColor = Taskbar->SolidColor
        }
    , Monitor->DesktopData->Taskbar->UiCtx->Cpnt);

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
        JsonObject* App = (JsonObject*) Taskbar->Applications->Get(i);

        uint8_t AppPosition = ((JsonNumber*) App->Get("position"))->Get();

/*         Button_t* AppButton = Button(
            {
                
            },
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
        , AppsContainer->Cpnt); */

        /* Picturebox_t* AppIcon = Picturebox("d0:kotlogo.tga", PictureboxType::_TGA,
            {
                .Fit = Ui::PICTUREFILL,
                .Transparency = true,
                .G{
                    .Width = -100, 
                    .Height = -100, 
                    .IsHidden = false
                }
            }
        , AppButton->Cpnt); */
    }

    Monitor->DesktopData->Taskbar->UiCtx->UiStartRenderer();

    return KSUCCESS;
}

KResult desktopc::RemoveMonitor(monitorc* Monitor){
    Monitor->Orb->Desktop = this;
    return KSUCCESS;
}

KResult desktopc::UpdateBackground(monitorc* Monitor){
    memcpy(Monitor->BackFramebuffer->Buffer, Monitor->DesktopData->Desktop->Fb->Buffer, Monitor->DesktopData->Desktop->Fb->Size);
    return KSUCCESS;
}

KResult desktopc::UpdateBackgroundEvent(monitorc* Monitor){
    BlitGraphicEventbuffer(Monitor->Eventbuffer, Monitor->DesktopData->Taskbar->Eventbuffer, Monitor->DesktopData->Taskbar->Position.x - Monitor->XPosition, Monitor->DesktopData->Taskbar->Position.y - Monitor->YPosition);
    return KSUCCESS;
}

KResult desktopc::UpdateWidgets(monitorc* Monitor){
    // Taskbar
    BlitFramebuffer(Monitor->BackFramebuffer, Monitor->DesktopData->Taskbar->Fb, Monitor->DesktopData->Taskbar->Position.x - Monitor->XPosition, Monitor->DesktopData->Taskbar->Position.y - Monitor->YPosition);
    return KSUCCESS;
}

KResult desktopc::UpdateWidgetsEvent(monitorc* Monitor){
    BlitGraphicEventbuffer(Monitor->Eventbuffer, Monitor->DesktopData->Taskbar->Eventbuffer, Monitor->DesktopData->Taskbar->Position.x - Monitor->XPosition, Monitor->DesktopData->Taskbar->Position.y - Monitor->YPosition);
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
    , Desktop->UiCtx->Cpnt);
}

void desktopmonitor::SetSolidColor(uint32_t Color){
    FillRect(Desktop->Fb, 0, 0, Desktop->Fb->Width, Desktop->Fb->Height, Color);
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

    if(kot_GetSecond()){
        kot_Sleep((60 - kot_GetSecond()) * 1000);
    }
    kot_GetActualTick(&TimerState);

    while(true){
        sprintf((char*)&TimeStr, "%d:%d", kot_GetHour(), kot_GetMinute());
        Time->UpdateText((char*)&TimeStr);
        sprintf((char*)&DateStr, "%d %s, %d", kot_GetDay(), MonthString[kot_GetMonth()-1], 2023);
        Date->UpdateText((char*)&DateStr);
        kot_SleepFromTick(&TimerState, 60000);
    }
    
    kot_Sys_Close(KSUCCESS);
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
    , Desktop->UiCtx->Cpnt);

    char TimeStr[1024];
    sprintf((char*)&TimeStr, "%d:%d", kot_GetHour(), kot_GetMinute());

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
    sprintf((char*)&DateStr, "%d %s, %d", kot_GetDay(), MonthString[kot_GetMonth()-1], 2023);

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

    kot_Sys_CreateThread(kot_Sys_GetProcess(), (void*)&UpdateClock, PriviledgeApp, NULL, &ClockThread);
    
    kot_arguments_t Parameters{
        .arg[0] = (uint64_t)Time,
        .arg[1] = (uint64_t)Date,
    };
    kot_Sys_ExecThread(ClockThread, &Parameters, ExecutionTypeQueu, NULL);
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

void desktopcomponent::MouseHandler(uint64_t PositionX, uint64_t PositionY, uint64_t ZValue, uint64_t Status){
    int64_t RelativePositionX = PositionX - Position.x;
    int64_t RelativePositionY = PositionY - Position.y;

    if(IsCpntFocus && (Status & MOUSE_CLICK_LEFT || Status & MOUSE_CLICK_MIDDLE || Status & MOUSE_CLICK_RIGHT || Status & MOUSE_CLICK_BUTTON4 || Status & MOUSE_CLICK_BUTTON5)){
        UiCtx->FocusCpnt->MouseEvent(UiCtx->FocusCpnt, true, RelativePositionX, RelativePositionY, PositionX, PositionY, ZValue, Status);
    }else{
        if(RelativePositionX >= 0 && RelativePositionY >= 0 && RelativePositionX < UiCtx->EventBuffer->Width && RelativePositionY < UiCtx->EventBuffer->Height){
            Ui::Component* Component = (Ui::Component*)GetEventData(UiCtx->EventBufferUse, RelativePositionX, RelativePositionY);
            if(Component){
                if(Component->MouseEvent){
                    Component->MouseEvent(Component, true, RelativePositionX, RelativePositionY, PositionX, PositionY, ZValue, Status);
                }
            }
        }else if(UiCtx->FocusCpnt){
            if(UiCtx->FocusCpnt->MouseEvent){
                UiCtx->FocusCpnt->MouseEvent(UiCtx->FocusCpnt, false, RelativePositionX, RelativePositionY, PositionX, PositionY, ZValue, Status);
                UiCtx->FocusCpnt = NULL;
            }
        }
        if(Status & MOUSE_CLICK_LEFT || Status & MOUSE_CLICK_MIDDLE || Status & MOUSE_CLICK_RIGHT || Status & MOUSE_CLICK_BUTTON4 || Status & MOUSE_CLICK_BUTTON5){
            IsCpntFocus = true;
        }else{
            IsCpntFocus = false;
        }
    }
}