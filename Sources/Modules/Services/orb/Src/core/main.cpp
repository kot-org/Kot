#include "main.h"

process_t self;

Context* screen_ctx = NULL;
Context* backbuffer_ctx = NULL;

void initBuffers() {
    srv_system_framebuffer_t* bootframebuffer = (srv_system_framebuffer_t*)malloc(sizeof(srv_system_framebuffer_t));
    Srv_System_GetFrameBuffer(bootframebuffer, true);

    framebuffer_t* screen = (framebuffer_t*) malloc(sizeof(framebuffer_t));
    screen->fb_size = bootframebuffer->pitch * bootframebuffer->height;
    uint64_t virtualAddress = (uint64_t)MapPhysical((uintptr_t)bootframebuffer->address, screen->fb_size);

    screen->fb_addr = virtualAddress;
    screen->width = bootframebuffer->width;
    screen->height = bootframebuffer->height;
    screen->pitch = bootframebuffer->pitch;
    screen->bpp = bootframebuffer->bpp;
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
    Sys_Close(KSUCCESS);
}

void initWindowRender() {
    windows = vector_create();
    Sys_Createthread(self, (uintptr_t)&threadRender, PriviledgeService, &renderThread);
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

thread_t CreateWindowThread = NULL;

/**
 * Return WindowID
 **/
void CreateWindow(uint64_t width, uint64_t height) {
    /* TODO */
    Sys_Close(NULL);
}

void initUISD() {
    Sys_Createthread(self, (uintptr_t) &CreateWindow, PriviledgeApp, &CreateWindowThread);
    uintptr_t address = getFreeAlignedSpace(sizeof(uisd_graphics_t));
    ksmem_t key = NULL;
    Sys_CreateMemoryField(self, sizeof(uisd_graphics_t), &address, &key, MemoryFieldTypeShareSpaceRO);
    uisd_graphics_t* OrbSrv = (uisd_graphics_t*) address;
    OrbSrv->ControllerHeader.IsReadWrite = false;
    OrbSrv->ControllerHeader.Version = Orb_Srv_Version;
    OrbSrv->ControllerHeader.VendorID = Kot_VendorID;
    OrbSrv->ControllerHeader.Type = ControllerTypeEnum_Graphics;
    OrbSrv->CreateWindow = MakeShareableThread(CreateWindowThread, PriviledgeApp);
    CreateControllerUISD(ControllerTypeEnum_Graphics, key, true);
}

extern "C" int main() {
    
    self = Sys_GetProcess();

    initBuffers();
    initWindowRender();

    drawLotLogo();

    //initUISD();

    Printlog("[ORB] Service initialized successfully");

    return KSUCCESS;

}