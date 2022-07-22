#include <core/main.h>

framebuffer_t* cloneFramebuffer(framebuffer_t* screen) {
    framebuffer_t* buffer = (framebuffer_t *) malloc(sizeof(framebuffer_t));
    buffer->fb_addr = (uint64_t)((uint8_t*) malloc(screen->fb_size));
    buffer->fb_size = screen->fb_size;
    buffer->width = screen->width;
    buffer->height = screen->height;
    buffer->bpp = screen->bpp;
    buffer->btpp = screen->btpp;
    buffer->bps = screen->bps;
    buffer->pitch = screen->pitch;
    return buffer;
}

extern "C" int main(int argc, char* argv[], bootbuffer_t* Framebuffer){

    Printlog("[FLOWGE] Initialization ...");

    kprocess_t self;
    SYS_GetProcessKey(&self);

    framebuffer_t* screen = (framebuffer_t*) malloc(sizeof(framebuffer_t));
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

    Context screen_ctx = Context(screen);
    Context backbuffer_ctx = Context(cloneFramebuffer(screen));
   
    // window logic

    Window w1(&backbuffer_ctx, 400, 400, 100, 100);
    w1.getContext()->clear();

    // render loop

    backbuffer_ctx.clearWith(0xff);
    // to optimize: swap back to front on another thread
    w1.render(&backbuffer_ctx);
    screen_ctx.swapFrom(&backbuffer_ctx);

    return KSUCCESS;

}