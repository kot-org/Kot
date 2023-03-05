#pragma once

#include <kot/sys.h>
#include <kot/stdio.h>

#include <kot/uisd/srvs/time.h>
#include <kot/uisd/srvs/storage.h>

#include <kot++/json.h>

#include <kot-graphics/orb.h>

#include <kot-ui++/context.h>
#include <kot-ui++/component.h>

using namespace std;

#define WIN_TASKBAR_COLOR      0x181818

class desktopc {
    private:
        Ui::UiContext* UiCtx;
        Ui::Picturebox_t* Wallpaper;
        framebuffer_t* Fb;
        thread_t ClockThread;

        typedef struct {
            uint8_t Height;
            window_t* Window;

            bool IsExtended;
            uint8_t IconSize;
        } Taskbar_t;

        Taskbar_t* Taskbar;

    public:
        desktopc(JsonObject* DesktopSettings);
        ~desktopc() { free(Taskbar); }

        void CreateTaskbar(JsonObject* TaskbarSettings);
        void ResizeTaskbar();

        void SetWallpaper(char* Path, Ui::PictureboxFit Fit);
        void SetSolidColor(uint32_t Color);

        void InitalizeClock(char* FontPath);
};