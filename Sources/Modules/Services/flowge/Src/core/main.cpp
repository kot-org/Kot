#include <core/main.h>

kprocess_t self;

Context* screen_ctx = NULL;
Context* backbuffer_ctx = NULL;

void initBuffers(bootbuffer_t* Framebuffer) {

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

    screen_ctx = new Context(screen);

    framebuffer_t* backbuffer = (framebuffer_t *) malloc(sizeof(framebuffer_t));
    backbuffer->fb_addr = (uint64_t)((uint8_t*) malloc(screen->fb_size));
    backbuffer->fb_size = screen->fb_size;
    backbuffer->width = screen->width;
    backbuffer->height = screen->height;
    backbuffer->bpp = screen->bpp;
    backbuffer->btpp = screen->btpp;
    backbuffer->pitch = screen->pitch;

    backbuffer_ctx = new Context(backbuffer);

}

vector_t* windows = NULL;
kthread_t renderThread = NULL;

void renderMain();

void initRender() {
    Sys_CreateThread(self, (uintptr_t)&renderMain, PriviledgeService, NULL, &renderThread);
    windows = vector_create(sizeof(Window));
}

void renderMain() {

}

extern "C" int main(int argc, char* argv[], bootbuffer_t* Framebuffer){

    Printlog("[FLOWGE] Initialization ...");

    Sys_GetProcessKey(&self);

    initBuffers(Framebuffer);
    initRender();

    Printlog("[FLOWGE] Service initialized successfully");

    // ## test ##

    Window* w1 = new Window(backbuffer_ctx, 400, 400, 250, 250);
    Window* w2 = new Window(backbuffer_ctx, 210, 210, 400, 100);
    Window* w3 = new Window(backbuffer_ctx, 210, 210, 1, 20);
    Window* w4 = new Window(backbuffer_ctx, 50, 50, 100, 50);

    vector_push(windows, w1);
    vector_push(windows, w2);
    vector_push(windows, w3);
    vector_push(windows, w4);

    w1->getContext()->clear();
    w2->getContext()->clear();
    w3->getContext()->clear();

    w2->getContext()->drawLine(10, 10, 200, 200, 0xffffff);
    w2->getContext()->drawLine(200, 10, 10, 200, 0xffffff);

    w3->getContext()->drawLine(10, 10, 200, 200, 0xffffff);
    w3->getContext()->drawLine(200, 10, 10, 200, 0xffffff);

    w1->getContext()->fillRect(0, 0, 10, 100, 0xff00ff);
    w1->getContext()->fillRect(50, 10, 100, 10, 0x00ff00);

    w1->getContext()->drawLine(200, 200, 250, 250, 0xffffff);
    w1->getContext()->drawLine(250, 260, 200, 210, 0xffffff);

    w1->getContext()->fillRect(10, 300, 70, 70, 0xffffff);
    w1->getContext()->drawRect(10, 300, 70, 70, 0xff0000);

    // draw a polygon with path function

    w1->getContext()->setAuto(true);
    w1->getContext()->abs_pos(150,150);
    w1->getContext()->rel_pos(20, 0);
    w1->getContext()->rel_pos(0, 20);
    w1->getContext()->rel_pos(-20, 0);
    w1->getContext()->rel_pos(-10, -10);
    w1->getContext()->draw(0xff00ff);
    w1->getContext()->reset();

    // fill the polygon with flood fill function

    w1->getContext()->fill(151, 151, 0xff00ff);
    
    w1->show();
    w2->show();
    w3->show();
    w4->show();
    
    // Sys_ExecThread(renderThread, NULL);

    backbuffer_ctx->clear();

    for (uint64_t i = 0; i < windows->length; i++) {
        ((Window*) vector_get(windows, i))->render(backbuffer_ctx);
    }

    screen_ctx->swapFrom(backbuffer_ctx);

    return KSUCCESS;

}