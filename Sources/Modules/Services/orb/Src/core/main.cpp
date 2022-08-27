#include "main.h"

process_t self;

vector_t* windows = NULL;
vector_t* monitors = NULL;

void renderWindows() {
    // todo: multi threads monitor rendering
    for (uint32_t i = 0; i < monitors->length; i++) {
        Monitor* monitor = (Monitor*) vector_get(monitors, i);
        if (monitor != NULL) {
            monitor->update(windows);
        }
    }
}

thread_t renderThread = NULL;

void threadRender(){
    renderWindows();
    Sys_Execthread(renderThread, NULL, ExecutionTypeQueu, NULL);
    Sys_Close(KSUCCESS);
}

void drawKotLogo() {

    Monitor* monitor0 = (Monitor*) vector_get(monitors, 0);
    Window* kotLogo = monitor0->getBackground();

    Context* ctx = new Context(kotLogo->getFramebuffer(), kotLogo->getWidth(), kotLogo->getHeight());

    ctx->clear();

    ctx->auto_pos(true);
    ctx->scale_pos(true);

    ctx->abs_pos(kotLogo->getWidth()/2 - 15 * ctx->get_scale(), kotLogo->getHeight()/2 - 50 * ctx->get_scale());
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
thread_t GetFramebufferThread = NULL;
thread_t GetWidthThread = NULL;
thread_t GetHeightThread = NULL;

/**
 * Return windowId
 **/
void CreateWindow(uint32_t width, uint32_t height, int32_t x, int32_t y) {
    Window* window = new Window(self, width, height, x, y);
    vector_push(windows, window);
    Sys_Close(windows->length-1);
}

/**
 * Return Framebuffer
 **/
void GetFramebuffer(uint32_t windowId) {
    Window* window = (Window*) vector_get(windows, windowId);
    if (window->getOwner() != Sys_GetProcess()) { Sys_Close(NULL); }
    Sys_Close(window->getFramebufferKey());
}

/**
 * Return Framebuffer
 **/
void GetHeight(uint32_t windowId) {
    Window* window = (Window*) vector_get(windows, windowId);
    Sys_Close(window->getHeight());
}

/**
 * Return Framebuffer
 **/
void GetWidth(uint32_t windowId) {
    Window* window = (Window*) vector_get(windows, windowId);
    Sys_Close(window->getWidth());
}

void initUISD() {

    Sys_Createthread(self, (uintptr_t) &CreateWindow, PriviledgeApp, &CreateWindowThread);
    Sys_Createthread(self, (uintptr_t) &GetFramebuffer, PriviledgeApp, &GetFramebufferThread);
    Sys_Createthread(self, (uintptr_t) &GetWidth, PriviledgeApp, &GetWidthThread);
    Sys_Createthread(self, (uintptr_t) &GetHeight, PriviledgeApp, &GetHeightThread);

    uintptr_t address = getFreeAlignedSpace(sizeof(uisd_graphics_t));
    ksmem_t key = NULL;
    Sys_CreateMemoryField(self, sizeof(uisd_graphics_t), &address, &key, MemoryFieldTypeShareSpaceRO);
    uisd_graphics_t* OrbSrv = (uisd_graphics_t*) address;
    OrbSrv->ControllerHeader.Process = self;
    OrbSrv->ControllerHeader.IsReadWrite = false;
    OrbSrv->ControllerHeader.Version = Orb_Srv_Version;
    OrbSrv->ControllerHeader.VendorID = Kot_VendorID;
    OrbSrv->ControllerHeader.Type = ControllerTypeEnum_Graphics;

    OrbSrv->CreateWindow = MakeShareableThread(CreateWindowThread, PriviledgeApp);
    OrbSrv->GetFramebuffer = MakeShareableThread(GetFramebufferThread, PriviledgeApp);
    OrbSrv->GetWidth = MakeShareableThread(GetWidthThread, PriviledgeApp);
    OrbSrv->GetHeight = MakeShareableThread(GetHeightThread, PriviledgeApp);

    CreateControllerUISD(ControllerTypeEnum_Graphics, key, true);

}

void initOrb() {

    self = ShareProcessKey(Sys_GetProcess());

    monitors = vector_create();
    windows = vector_create();

    srv_system_framebuffer_t* bootframebuffer = (srv_system_framebuffer_t*) malloc(sizeof(srv_system_framebuffer_t));
    Srv_System_GetFrameBuffer(bootframebuffer, true);

    size64_t fb_size = bootframebuffer->pitch * bootframebuffer->height;
    
    uint64_t virtualAddress = (uint64_t)MapPhysical((uintptr_t)bootframebuffer->address, fb_size);

    Monitor* monitor0 = new Monitor((uintptr_t) virtualAddress, bootframebuffer->width, bootframebuffer->height, 0, 0);

    free(bootframebuffer);
    
    vector_push(monitors, monitor0);

    drawKotLogo();

    Sys_Createthread(self, (uintptr_t) &threadRender, PriviledgeService, &renderThread);
    Sys_Execthread(renderThread, NULL, ExecutionTypeQueu, NULL);

}

extern "C" int main() {

    initOrb();
    initUISD();

    Printlog("[ORB] Service started");

    return KSUCCESS;

}