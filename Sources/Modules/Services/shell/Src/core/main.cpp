#include "main.h"

#include <kot++/java/se8/jvm.h>
using namespace SE8;

uintptr_t fb;
uint32_t fb_height;
psf1_font* zap_light16;

void shell_print(char* str) {
    static uint32_t x = 0;
    static uint32_t y = 10;
    psf1_print(zap_light16, fb, fb_height, str, &x, &y, 0xffffffff);
}

extern "C" int main() {

    uint32_t wid = orb::Create(300, 300, 10, 10);
    fb = orb::GetFramebuffer(wid);
    fb_height = 300;

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