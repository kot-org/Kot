#pragma once

#include <kot/sys.h>

#include <kot++/json.h>

#include <kot/uisd/srvs/storage.h>

#include <kot-graphics/orb.h>
#include <kot-ui++/component.h>

using namespace std;

class desktopc {
    public:
        desktopc(JsonArray* Settings);

        void SetWallpaper(char* Path);
        void SetSolidColor(uint32_t Color);

    private:
        framebuffer_t* Fb;
};