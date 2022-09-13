#include <core/main.h>

extern "C" int main() {
    std::framebuffer_t* fb;

    uint32_t wid = orb::Create(500, 300, 10, 400);
    fb = orb::GetFramebuffer(wid);
    orb::Show(wid);

    srv_system_callback_t* callback1 = Srv_System_ReadFileInitrd("default-font.sfn", true);
    kfont_t* font = LoadFont(callback1->Data);
    free(callback1);
    font_fb_t fontBuff;
    fontBuff.address = fb->addr;
    fontBuff.width = fb->width;
    fontBuff.height = fb->height;
    fontBuff.pitch = fb->pitch;
    LoadPen(font, &fontBuff, 0, 0, 16, 0, 0xFFFFFFFF);
    DrawFont(font, "demo");
    EditPen(font, NULL, -1, -1, 64, -1, -1);
    DrawFont(font, "demo");
    FreeFont(font);

    return KSUCCESS;
}