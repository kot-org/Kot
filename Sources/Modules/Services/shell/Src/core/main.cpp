#include <core/main.h>

#include <kot++/java/se8/jvm.h>
using namespace SE8;

#include <kot++/printf.h>
using namespace std;

std::framebuffer_t* fb;
kfont_t* font;

void shell_print(char* str) {
    DrawFont(font, str);
}

extern "C" int main() {
    uint32_t wid = orb::Create(300, 300, 10, 10);
    fb = orb::GetFramebuffer(wid);
    orb::Show(wid);

    srv_system_callback_t* callback0 = Srv_System_ReadFileInitrd("default-font.sfn", true);
    font = LoadFont(callback0->Data);
    free(callback0);
    font_fb_t fontBuff;
    fontBuff.address = fb->addr;
    fontBuff.width = fb->width;
    fontBuff.height = fb->height;
    fontBuff.pitch = fb->pitch;
    SetFont(font, &fontBuff, 0, 0, 0xFFFFFF, NULL, 16, 0);

    JavaVM* vm = new JavaVM();
    vm->setOutput(&shell_print);
    srv_system_callback_t* callback1 = Srv_System_ReadFileInitrd("Test.class", true);
    vm->loadClassBytes(callback1->Data);
    free(callback1);
    vm->setEntryPoint("Test");
    vm->run(NULL, 0);

    return KSUCCESS;

}