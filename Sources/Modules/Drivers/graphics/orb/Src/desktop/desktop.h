#pragma once

#include <core/main.h>

#include <kot++/json.h>
#include <kot-ui++/context.h>
#include <kot-ui++/component.h>
#include <kot-audio++/stream.h>

class orbc;
class windowc;
class monitorc;
class desktopc;
class renderc;
class hidc;

using namespace std;
using namespace Ui;

class desktopc{
    public:
        desktopc(orbc* Parent);
        KResult AddMonitor(monitorc* Monitor);
        KResult RemoveMonitor(monitorc* Monitor);
        KResult UpdateBackground(monitorc* Monitor);
        KResult UpdateBackgroundEvent(monitorc* Monitor);
        KResult UpdateWidgets(monitorc* Monitor);
        KResult UpdateWidgetsEvent(monitorc* Monitor);
    private:
        void UpdateTaskbar(JsonObject* TaskbarSettings);

        bool IsWallpaper;
        char* WallpaperPath;
        uint8_t WallpaperFit;
        uint32_t SolidColor;
        bool IsClock;
        char* FontPathClock;

        typedef struct {
            uint8_t Height;

            bool IsExtended;
            uint8_t IconSize;
            uint32_t SolidColor;

            JsonArray* Applications;
        } Taskbar_t;

        Taskbar_t* Taskbar;
};

struct desktopcomponent{
    Ui::UiContext* UiCtx;
    framebuffer_t* Fb;
    graphiceventbuffer_t* Eventbuffer;
    struct hid_event_t* MouseEvent;
    thread_t MouseEventThread;
    bool IsCpntFocus;
    point_t Position;
    void MouseHandler(uint64_t PositionX, uint64_t PositionY, uint64_t ZValue, uint64_t Status);
};

struct desktopmonitor{
    desktopcomponent* Desktop;
    desktopcomponent* Taskbar;

    Ui::Picturebox_t* Wallpaper;
    desktopc* Parent;
    thread_t ClockThread;

    void SetWallpaper(char* Path, Ui::PictureboxFit Fit);
    void SetSolidColor(uint32_t Color);
    void InitalizeClock(char* FontPath);
};