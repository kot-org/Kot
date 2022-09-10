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

    // _ [] X buttons
    std::drawLine(fb, fb->width-17, 17, fb->width-7, 7, 0xffffff);
    std::drawLine(fb, fb->width-7, 17, fb->width-17, 7, 0xffffff);
    std::drawRect(fb, fb->width-35, 7, 10, 10, 0xffffff);
    std::drawLine(fb, fb->width-53, 17, fb->width-43, 17, 0xffffff);

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

    srv_system_callback_t* callback3 = Srv_System_ReadFileInitrd("UbuntuBold.sfn", true);
    kfont_t* font = LoadFont(callback3->Data);
    free(callback3);
    font_fb_t* fontBuff = (font_fb_t*) malloc(sizeof(font_fb_t));
    fontBuff->address = fb->addr;
    std::printf("fb: %x", fontBuff->address);
    fontBuff->width = fb->width;
    std::printf("width: %x", fontBuff->width);
    fontBuff->height = fb->height;
    std::printf("height: %x", fontBuff->height);
    fontBuff->pitch = fb->pitch;
    std::printf("pitch: %x", fontBuff->pitch);
    PrintFont(font, "hi im seb.", fontBuff, 0, 50, 0xFFFFFFFF);
    free(fontBuff);
    FreeFont(font);

    return KSUCCESS;

}