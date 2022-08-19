#include "main.h"

process_t self;

Context* screen_ctx = NULL;
Context* backbuffer_ctx = NULL;

void initBuffers(bootbuffer_t* fb) {

    framebuffer_t* screen = (framebuffer_t*) malloc(sizeof(framebuffer_t));
    screen->fb_size = fb->framebuffer_pitch * fb->framebuffer_height;

    uint64_t virtualAddress = (uint64_t) KotSpecificData.FreeMemorySpace - screen->fb_size;
    SYS_Map(self, &virtualAddress, true, (uintptr_t *) &fb->framebuffer_addr, &screen->fb_size, false);

    screen->fb_addr = virtualAddress;
    screen->width = fb->framebuffer_width;
    screen->height = fb->framebuffer_height;
    screen->pitch = fb->framebuffer_pitch;
    screen->bpp = fb->framebuffer_bpp;
    screen->btpp = screen->bpp/8;

    screen_ctx = new Context(screen);
    atomicUnlock((uint64_t*) screen_ctx, 0);

    framebuffer_t* backbuffer = (framebuffer_t *) malloc(sizeof(framebuffer_t));
    backbuffer->fb_addr = (uint64_t)((uint8_t*) malloc(screen->fb_size));
    backbuffer->fb_size = screen->fb_size;
    backbuffer->width = screen->width;
    backbuffer->height = screen->height;
    backbuffer->bpp = screen->bpp;
    backbuffer->btpp = screen->btpp;
    backbuffer->pitch = screen->pitch;

    backbuffer_ctx = new Context(backbuffer);
    atomicUnlock((uint64_t*) backbuffer_ctx, 0);

}

vector_t* windows = NULL;

void renderWindows() {

    backbuffer_ctx->clear();

    for (uint64_t i = 0; i < windows->length; i++) {
        ((Window*) vector_get(windows, i))->render(backbuffer_ctx);
    }

    screen_ctx->swapFrom(backbuffer_ctx);

}

thread_t renderThread = NULL;

void threadRender(){
    renderWindows();
    Sys_Execthread(renderThread, NULL, ExecutionTypeQueu, NULL);
    SYS_Exit(NULL, KSUCCESS);
}

void initWindowRender() {
    windows = vector_create();
    Sys_Createthread(self, (uintptr_t)&threadRender, PriviledgeService, NULL, &renderThread);
    Sys_Execthread(renderThread, NULL, ExecutionTypeQueu, NULL);
}

void drawLotLogo() {

    Window* kotLogo = new Window(backbuffer_ctx, backbuffer_ctx->getFramebuffer()->width-1, backbuffer_ctx->getFramebuffer()->height-1, 0, 0);
    
    vector_push(windows, kotLogo);

    kotLogo->hideBorders();
    kotLogo->show();

    Context* ctx = kotLogo->getContext();

    ctx->clear();

    ctx->auto_pos(true);
    ctx->scale_pos(true);

    ctx->abs_pos(ctx->getFramebuffer()->width/2 - 15 * ctx->get_scale(), ctx->getFramebuffer()->height/2 - 50 * ctx->get_scale());
    ctx->rel_pos(0, 75);
    ctx->rel_pos(10, -5);
    ctx->rel_pos(0, -25);
    ctx->rel_pos(13, 0);
    ctx->rel_pos(0, 33);
    ctx->rel_pos(10, -5);
    ctx->rel_pos(0, -38);
    ctx->rel_pos(-13, 0);
    ctx->rel_pos(15, -17);
    ctx->rel_pos(-10, -3);
    ctx->rel_pos(-15, 17);
    ctx->rel_pos(0, -37);

    ctx->draw(0xffffff);
    ctx->fill(ctx->get_pos(0)->x+1, ctx->get_pos(0)->y+1, 0xffffff);

}

extern "C" int main(int argc, char* argv[], bootbuffer_t* fb){

    Printlog("[ORB] Initialization ...");
    
    Sys_GetProcessKey(&self);

    initBuffers(fb);
    initWindowRender();

    drawLotLogo();

    Printlog("[ORB] Service initialized successfully");

    // ## test ##

    Window* w1 = new Window(backbuffer_ctx, 400, 400, 410, 410);
    Window* w2 = new Window(backbuffer_ctx, 50, 50, 100, 50);

    vector_push(windows, w1);
    vector_push(windows, w2);

    w1->getContext()->clear();
    w2->getContext()->clear();

    w1->getContext()->fillRect(0, 0, 10, 100, 0xff00ff);
    w1->getContext()->fillRect(50, 10, 100, 10, 0x00ff00);

    w1->getContext()->drawLine(200, 200, 250, 250, 0xffffff);
    w1->getContext()->drawLine(250, 260, 200, 210, 0xffffff);

    w1->getContext()->fillRect(10, 300, 70, 70, 0xffffff);
    w1->getContext()->drawRect(10, 300, 70, 70, 0xff0000);

    w1->getContext()->auto_pos(true);
    w1->getContext()->abs_pos(150,150);
    w1->getContext()->rel_pos(20, 0);
    w1->getContext()->rel_pos(0, 20);
    w1->getContext()->rel_pos(-20, 0);
    w1->getContext()->rel_pos(-10, -10);
    w1->getContext()->draw(0xff00ff);
    w1->getContext()->reset();

    w1->getContext()->fill(151, 151, 0xff00ff);

    w1->getContext()->drawCircle(100, 100, 20, 0xffff00);
    
    w1->show();
    w2->show();

    for (uint32_t i = 0; i < 400; i++) {
        w1->move(w1->getX()-1, w1->getY()-1);
    }
 
    return KSUCCESS;

}