#include <core/main.h>

#include <kot-ui++/window.h>

#include <kot++/printf.h>

extern "C" int main() {
    UiWindow::Window("test", 500, 300, 10, 400);

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