#include <core/main.h>

#include <kot-ui++/window.h>

#include <kot++/printf.h>

#include <kot/uisd/srvs/system.h>

using namespace Ui;

typedef struct {
    uint8_t idLength;
    uint8_t colorMapType;
    uint8_t imageType;
    uint16_t colorMapOrigin, colorMapLength;
    uint8_t colorMapEntSz;
    uint16_t x, y;
    uint16_t width, height;
    uint8_t bpp;
    uint8_t imageDescriptor;
} __attribute__((packed)) tgaHeader_t;

void TGA(char* path) {
    srv_system_callback_t* file = Srv_System_ReadFileInitrd(path, true);
    tgaHeader_t *data = (tgaHeader_t*) file->Data;

    if(!data) return;

    switch (data->imageType)
    {
        case 1:

            break;
        
        case 2:
        {
            uint64_t imageDataOffset = data->colorMapOrigin + data->colorMapLength;

            data->bpp = 32;

            uint8_t btpp = data->bpp/8;
            uint32_t pitch = data->width * (btpp);

            std::printf("width: %d", data->width);
            std::printf("height: %d", data->height);
            std::printf("bpp: %d", data->bpp);
            std::printf("btpp: %d", btpp);

            break;
        }
        case 9:
        
            break;
        
        case 10:
        
            break;
    }
}

extern "C" int main() {
    UiWindow::Window("test", 500, 300, 10, 400);

    //TGA("sample.tga");

    // titlebar_t* titlebar = CreateTitleBar("test", GetMainParent(fb), { .bgColor = 0xFF1B1B1B, .visible = true });

/*     srv_system_callback_t* callback1 = Srv_System_ReadFileInitrd("default-font.sfn", true);
    kfont_t* font = LoadFont(callback1->Data);
    free(callback1);
    font_fb_t fontBuff;
    fontBuff.address = fb->addr;
    fontBuff.width = fb->width;
    fontBuff.height = fb->height;
    fontBuff.pitch = fb->pitch;
    LoadPen(font, &fontBuff, 0, 0, 8, 0, 0xFFFFFFFF);
    DrawFont(font, "demo");
    EditPen(font, NULL, -1, 32, 16, -1, -1);
    DrawFont(font, "demo");
    EditPen(font, NULL, -1, 64, 32, -1, -1);
    DrawFont(font, "demo");
    EditPen(font, NULL, -1, 128, 64, -1, -1);
    DrawFont(font, "demo");
    FreeFont(font); */

    return KSUCCESS;
}