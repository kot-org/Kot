#include <core/main.h>

#include <kot++/java/se8/jvm.h>
using namespace SE8;

#include <kot++/printf.h>
using namespace std;

Graphic::framebuffer_t* Framebuffer;
kfont_t* Font;

void Print(char* str){
    DrawFont(Font, str);
}

extern "C" int main() {
    Graphic::framebuffer_t Backbuffer;
    uint32_t wid = orb::create(300, 300, 10, 10);
    Framebuffer = orb::getFramebuffer(wid);
    orb::show(wid);
    Backbuffer.addr = calloc(Framebuffer->pitch * Framebuffer->height);
    Backbuffer.width = Framebuffer->width;
    Backbuffer.height = Framebuffer->height;
    Backbuffer.pitch = Framebuffer->pitch;
    Backbuffer.btpp = Framebuffer->btpp;
    
    file_t* FontFile = fopen("default-font.sfn", "r");
    fseek(FontFile, 0, SEEK_END);
    size64_t Size = ftell(FontFile);
    uintptr_t Buffer = malloc(Size);
    fseek(FontFile, 0, SEEK_SET);
    fread(Buffer, Size, 1, FontFile);
    Font = (kfont_t*)LoadFont(Buffer);

    font_fb_t FontFB;
    FontFB.address = Backbuffer.addr;
    FontFB.width = Backbuffer.width;
    FontFB.height = Backbuffer.height;
    FontFB.pitch = Backbuffer.pitch;
    LoadPen(Font, &FontFB, 0, 0, 16, 0, 0xFFFFFFFF);

    uint64_t TimerState;
    GetActualTick(&TimerState);
    
    while (true){
        fillRect(&Backbuffer, 0, 0, 10, 16, 0xffffff);
        blitFramebuffer(Framebuffer, &Backbuffer, 0, 0);
        SleepFromTick(&TimerState, 500);
        fillRect(&Backbuffer, 0, 0, 10, 16, 0x0);
        blitFramebuffer(Framebuffer, &Backbuffer, 0, 0);
        SleepFromTick(&TimerState, 500);
    }

    return KSUCCESS;
}