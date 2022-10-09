#include <core/main.h>

#include <kot++/java/se8/jvm.h>
using namespace SE8;

#include <kot++/printf.h>
using namespace std;

std::framebuffer_t* fb;
kfont_t* font;
uint64_t line = 16;

void shell_print(char* str){
    DrawFont(font, str);
}

extern "C" int main() {
    
    uint32_t wid = orb::create(300, 300, 10, 10);
    fb = orb::getFramebuffer(wid);
    orb::show(wid);

    srv_system_callback_t* callback0 = Srv_System_ReadFileInitrd("default-font.sfn", true);
    font = LoadFont((uintptr_t)callback0->Data);
    free(callback0);
    font_fb_t fontBuff;
    fontBuff.address = fb->addr;
    fontBuff.width = fb->width;
    fontBuff.height = fb->height;
    fontBuff.pitch = fb->pitch;
    LoadPen(font, &fontBuff, 0, 0, 16, 0, 0xFFFFFFFF);

    // _ [] X buttons
    std::drawLine(fb, fb->width-17, 17, fb->width-7, 7, 0xffffff);
    std::drawLine(fb, fb->width-7, 17, fb->width-17, 7, 0xffffff);
    std::drawRect(fb, fb->width-35, 7, 10, 10, 0xffffff);
    std::drawLine(fb, fb->width-53, 17, fb->width-43, 17, 0xffffff);

    JavaVM* vm = new JavaVM();
    vm->setOutput(&shell_print);
    srv_system_callback_t* callback1 = Srv_System_ReadFileInitrd("Test.class", true);
    vm->loadClassBytes((uintptr_t)callback1->Data);
    free(callback1);
    vm->setEntryPoint("Test");
    vm->run(NULL, 0);

    return KSUCCESS;
}