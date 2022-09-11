#include <core/main.h>

extern "C" int main() {
    std::framebuffer_t* fb;

    uint32_t wid = orb::Create(500, 300, 10, 400);
    fb = orb::GetFramebuffer(wid);
    orb::Show(wid);

    srv_system_callback_t* callback1 = Srv_System_ReadFileInitrd("default-font.sfn", true);
    kfont_t* font = LoadFont(callback1->Data);
    free(callback1);
    font_fb_t* fontBuff = (font_fb_t*) malloc(sizeof(font_fb_t));
    fontBuff->address = fb->addr;
    fontBuff->width = fb->width;
    fontBuff->height = fb->height;
    fontBuff->pitch = fb->pitch;
    PrintFont(font, "demo", fontBuff, 0, 0, 16, 0xFFFFFFFF);
    free(fontBuff);
    FreeFont(font);

    return KSUCCESS;
}