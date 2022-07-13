#include "core/main.h"
#include "window/window.h"

uint64_t fb_addr;
size_t fb_size;
uint16_t fb_width;
uint16_t fb_height;
uint16_t fb_pitch;
uint16_t fb_bpp;

int main(int argc, char* argv[], framebuffer_t* Framebuffer)
{
    Printlog("[WINDOWS MANAGER] Initialization ...");
    
    /* Init and Map framebuffer */
    kprocess_t self;
    uint64_t virtualAddress = KotSpecificData.FreeMemorySpace - (Framebuffer->framebuffer_pitch * Framebuffer->framebuffer_height);
    fb_size = Framebuffer->framebuffer_pitch * Framebuffer->framebuffer_height;
    
    SYS_GetProcessKey(&self);
    SYS_Map(self, &virtualAddress, true, &Framebuffer->framebuffer_addr, &fb_size, false);
    Framebuffer->framebuffer_addr = virtualAddress;

    fb_addr = Framebuffer->framebuffer_addr;
    fb_width = Framebuffer->framebuffer_width;
    fb_height = Framebuffer->framebuffer_height;
    fb_pitch = Framebuffer->framebuffer_pitch;
    fb_bpp = Framebuffer->framebuffer_bpp;

    clear_screen();

    create_window(50, 50, 400, 200);

    return KSUCCESS;
}