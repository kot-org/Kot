#include <core/main.h>

#include <kot++/java/se8/jvm.h>
using namespace SE8;

#include <kot++/printf.h>
using namespace std;

Graphic::framebuffer_t* fb;
kfont_t* font;
uint64_t line = 16;

void shell_print(char* str){
    DrawFont(font, str);
}

extern "C" int main() {
    Graphic::framebuffer_t backfb;
    uint32_t wid = orb::create(300, 300, 10, 10);
    fb = orb::getFramebuffer(wid);
    orb::show(wid);
    backfb.addr = calloc(fb->pitch * fb->height);
    backfb.width = fb->width;
    backfb.height = fb->height;
    backfb.pitch = fb->pitch;

    srv_system_callback_t* callback0 = Srv_System_ReadFileInitrd("default-font.sfn", true);
    font = LoadFont((uintptr_t)callback0->Data);
    free(callback0);
    font_fb_t fontBuff;
    fontBuff.address = backfb.addr;
    fontBuff.width = backfb.width;
    fontBuff.height = backfb.height;
    fontBuff.pitch = backfb.pitch;
    LoadPen(font, &fontBuff, 0, 0, 16, 0, 0xFFFFFFFF);

    uint64_t TimerState;
    GetActualTick(&TimerState);

    char buffer[33];
    while (true){
        StringBuilder* strBuilder = new StringBuilder();
        strBuilder->append((char*)itoa(GetYear(), (char*)&buffer, 10));
        strBuilder->append("/");
        strBuilder->append((char*)itoa(GetMonth(), (char*)&buffer, 10));
        strBuilder->append("/");
        strBuilder->append((char*)itoa(GetDay(), (char*)&buffer, 10));
        strBuilder->append(" - ");
        strBuilder->append((char*)itoa(GetHour(), (char*)&buffer, 10));
        strBuilder->append(":");
        strBuilder->append((char*)itoa(GetMinute(), (char*)&buffer, 10));
        strBuilder->append(":");
        strBuilder->append((char*)itoa(GetSecond(), (char*)&buffer, 10));
        memset(backfb.addr, 0, backfb.pitch * backfb.height);
        DrawFont(font, strBuilder->toString());
        memcpy(fb->addr, backfb.addr, backfb.pitch * backfb.height);
        free(strBuilder);
        EditPen(font, NULL, 0, 0, -1, -1, -1);
        SleepFromTick(&TimerState, 1000);
    }

    // // _ [] X buttons
    // Graphic::drawLine(fb, fb->width-17, 17, fb->width-7, 7, 0xffffff);
    // Graphic::drawLine(fb, fb->width-7, 17, fb->width-17, 7, 0xffffff);
    // Graphic::drawRect(fb, fb->width-35, 7, 10, 10, 0xffffff);
    // Graphic::drawLine(fb, fb->width-53, 17, fb->width-43, 17, 0xffffff);

    // JavaVM* vm = new JavaVM();
    // vm->setOutput(&shell_print);
    // srv_system_callback_t* callback1 = Srv_System_ReadFileInitrd("Test.class", true);
    // vm->loadClassBytes((uintptr_t)callback1->Data);
    // free(callback1);
    // vm->setEntryPoint("Test");
    // vm->run(NULL, 0);

    return KSUCCESS;
}