#include <core/main.h>

#include <kot-ui/component.h>

#include <kot++/printf.h>

extern "C" int main() {
    std::framebuffer_t* fb;

    uint32_t wid = orb::create(500, 300, 10, 400);
    fb = orb::getFramebuffer(wid);
    orb::show(wid);

    ctxg_t* ctxGraphic = CreateGraphicContext(fb->addr, fb->width, fb->height);
    ctxui_t* ctxUi = CreateUiContext((framebuffer_t*)fb);
    // canva_t* canva1 = CreateCanva(200, 100, GetMainParent((framebuffer_t*)fb));
    titlebar_t* titlebar = CreateTitleBar("test", 0, 0, 0xFF1B1B1B, true, GetMainParent((framebuffer_t*)fb));
    //UpdateContext(ctxUi);

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