#include <core/main.h>

void clear(screen_t* screen);

extern "C" int main(framebuffer_t* Framebuffer){

    Printlog("[FLOWGE] Initialization ...");

    kprocess_t self;
    SYS_GetProcessKey(&self);

    screen_t* screen = (screen_t*) malloc(sizeof(screen_t));
    screen->fb_size = Framebuffer->framebuffer_pitch * Framebuffer->framebuffer_height;

    uint64_t virtualAddress = (uint64_t) KotSpecificData.FreeMemorySpace - screen->fb_size;
    SYS_Map(self, &virtualAddress, true, (uintptr_t *) &Framebuffer->framebuffer_addr, &screen->fb_size, false);

    screen->fb_addr = virtualAddress;
    screen->width = Framebuffer->framebuffer_width;
    screen->height = Framebuffer->framebuffer_height;
    screen->pitch = Framebuffer->framebuffer_pitch;
    screen->bpp = Framebuffer->framebuffer_bpp;
    screen->btpp = screen->bpp/8;
    screen->bps = screen->btpp * screen->width;

    return KSUCCESS;

}