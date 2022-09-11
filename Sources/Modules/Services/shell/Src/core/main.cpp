#include <core/main.h>

#include <kot++/java/se8/jvm.h>
using namespace SE8;

#include <kot++/printf.h>
using namespace std;

std::framebuffer_t* fb;
psf1_font* zap_light16;

void shell_print(char* str) {
    static uint32_t x = 0;
    static uint32_t y = 10;
    psf1_print(zap_light16, fb->addr, fb->height, str, &x, &y, 0xffffff);
}

extern "C" int main() {

    uint32_t wid = orb::Create(300, 300, 10, 10);
    fb = orb::GetFramebuffer(wid);
    orb::Show(wid);

    srv_system_callback_t* callback0 = Srv_System_ReadFileInitrd("default-font.sfn", true);
    kfont_t* font = LoadFont(callback0->Data);
    free(callback0);
    font_fb_t fontBuff;
    fontBuff.address = fb->addr;
    fontBuff.width = fb->width;
    fontBuff.height = fb->height;
    fontBuff.pitch = fb->pitch;
    PrintFont(font, "hello world.\ntest", &fontBuff, 0, 0, 36, 0xFFFFFFFF);
    FreeFont(font);

    srv_system_callback_t* callback1 = Srv_System_ReadFileInitrd("zap-light16.psf", true);
    zap_light16 = psf1_parse(callback1->Data);
    free(callback1);

    JavaVM* vm = new JavaVM();
    vm->setOutput(&shell_print);
    srv_system_callback_t* callback2 = Srv_System_ReadFileInitrd("Test.class", true);
    vm->loadClassBytes(callback2->Data);
    free(callback2);
    vm->setEntryPoint("Test");
    vm->run(NULL, 0);

    return KSUCCESS;

}