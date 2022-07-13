#include <core/main.h>
#include <window/window.h>

ScreenInfo* screenInfo;

int main(int argc, char* argv[], framebuffer_t* Framebuffer)
{
    Printlog("[WINDOWS MANAGER] Initialization ...");
    
    /* Init and Map framebuffer */
    screenInfo = (ScreenInfo *) malloc(sizeof(ScreenInfo));
    kprocess_t self;
    uint64_t virtualAddress = KotSpecificData.FreeMemorySpace - (Framebuffer->framebuffer_pitch * Framebuffer->framebuffer_height);
    screenInfo->fb_size = Framebuffer->framebuffer_pitch * Framebuffer->framebuffer_height;
    
    SYS_GetProcessKey(&self);
    SYS_Map(self, &virtualAddress, true, &Framebuffer->framebuffer_addr, &screenInfo->fb_size, false);
    Framebuffer->framebuffer_addr = virtualAddress;

    screenInfo->fb_addr = Framebuffer->framebuffer_addr;
    screenInfo->width = Framebuffer->framebuffer_width;
    screenInfo->height = Framebuffer->framebuffer_height;
    screenInfo->pitch = Framebuffer->framebuffer_pitch;
    screenInfo->bpp = Framebuffer->framebuffer_bpp;

    ClearScreen();

    CreateWindow(50, 50, 400, 200);

    return KSUCCESS;
}