#include <core/main.h>
#include <window/window.h>

window_t* screenInfo;

int main(int argc, char* argv[], framebuffer_t* Framebuffer)
{   
    Printlog("[WINDOWS MANAGER] Initialization ...");
    
    /* Init and Map framebuffer */
    kprocess_t self;
    uint64_t virtualAddress = KotSpecificData.FreeMemorySpace - (Framebuffer->framebuffer_pitch * Framebuffer->framebuffer_height);

    screenInfo = (window_t*) malloc(sizeof(window_t));
    screenInfo->fb_size = Framebuffer->framebuffer_pitch * Framebuffer->framebuffer_height;

    SYS_GetProcessKey(&self);
    SYS_Map(self, &virtualAddress, true, &Framebuffer->framebuffer_addr, &screenInfo->fb_size, false);

    /* Init struct */
    screenInfo->fb_addr = virtualAddress;
    screenInfo->width = Framebuffer->framebuffer_width;
    screenInfo->height = Framebuffer->framebuffer_height;
    screenInfo->pitch = Framebuffer->framebuffer_pitch;
    screenInfo->bpp = Framebuffer->framebuffer_bpp;

    ClearScreen();

    /* 
        TODO: Pour Yira -> faire systeme pour optimiser 
    */
    window_t* window1 = CreateWindow(50, 50, 400, 200);

    // MoveWindow(window1, 400, 400);
    CloseWindow(window1);
    MoveWindow(window1, 400, 400);

    return KSUCCESS;
}