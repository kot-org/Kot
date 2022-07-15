#include <core/main.h>

screen_t* screen;

int main(int argc, char* argv[], framebuffer_t* Framebuffer) {   

    Printlog("[FLOWGE] Initialization ...");
    
    kprocess_t self;

    screen = (screen_t*) malloc(sizeof(screen_t));
    screen->fb_size = Framebuffer->framebuffer_pitch * Framebuffer->framebuffer_height;
    
    uint64_t virtualAddress = KotSpecificData.FreeMemorySpace - screen->fb_size;

    SYS_GetProcessKey(&self);
    SYS_Map(self, &virtualAddress, true, &Framebuffer->framebuffer_addr, &screen->fb_size, false);

    screen->fb_addr = virtualAddress;
    screen->width = Framebuffer->framebuffer_width;
    screen->height = Framebuffer->framebuffer_height;
    screen->pitch = Framebuffer->framebuffer_pitch;
    screen->bpp = Framebuffer->framebuffer_bpp;
    screen->pixel_width = screen->bpp/8;
    screen->bps = screen->pixel_width * screen->width;

    clear(screen);

    // gray rectangle with white border
    fillRect(screen, 100, 100, 200, 200, 0x1A1A1A);
    drawRect(screen, 100, 100, 200, 200, 0xFFFFFF);

    drawLine(screen, 10, 10, 100, 100, 0xFFFFFF);
    drawLine(screen, 200, 11, 170, 85, 0xFF00FF);
    drawLine(screen, 170, 85, 180,300, 0xFF00FF);

    return KSUCCESS;

}