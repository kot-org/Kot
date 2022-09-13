#include <core/main.h>

#include <kot-ui/window.h>

extern "C" int main() {
    std::framebuffer_t* fb;

    uint32_t wid = orb::Create(500, 300, 10, 400);
    fb = orb::GetFramebuffer(wid);
    orb::Show(wid);
    CreateTitleBar(fb);

    srv_system_callback_t* callback1 = Srv_System_ReadFileInitrd("default-font.sfn", true);
    kfont_t* font = LoadFont(callback1->Data);
    free(callback1);
    font_fb_t fontBuff;
    fontBuff.address = fb->addr;
    fontBuff.width = fb->width;
    fontBuff.height = fb->height;
    fontBuff.pitch = fb->pitch;
    LoadPen(font, &fontBuff, 0, 0, 8, 0, 0xFFFFFFFF);
    DrawFont(font, "demo");
    EditPen(font, NULL, 0, 32, 16, 0, 0);
    DrawFont(font, "demo");
    EditPen(font, NULL, 0, 64, 32, 0, 0);
    DrawFont(font, "demo");
    EditPen(font, NULL, 0, 128, 64, 0, 0);
    DrawFont(font, "demo");
    FreeFont(font);

    return KSUCCESS;
}