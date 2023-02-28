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

class desktopc {
    public:
        desktopc(JsonArray* Settings);

        void InitalizeClock(char* FontPath);
        void SetWallpaper(char* Path, Ui::PictureboxFit Fit);
        void SetSolidColor(uint32_t Color);

    private:
        Ui::UiContext* UiCtx;
        Ui::Picturebox_t* Wallpaper;
        framebuffer_t* Fb;
        thread_t ClockThread;

        typedef enum {
            FIT     = 0,
            FILL    = 1,
            CENTER  = 2,
            STRETCH = 3,
        } WallpaperFit;
};