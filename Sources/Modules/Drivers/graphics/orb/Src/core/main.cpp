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

thread_t createThread = NULL;
thread_t destroyThread = NULL;
thread_t getFramebufferThread = NULL;
thread_t getWidthThread = NULL;
thread_t getHeightThread = NULL;
thread_t showThread = NULL;
thread_t hideThread = NULL;
thread_t resizeThread = NULL;
thread_t moveThread = NULL;

/**
 * Return windowId
 **/
void create(uint32_t width, uint32_t height, uint32_t x, uint32_t y) {
    Window* window = new Window(self, width, height, x, y);
    vector_push(windows, window);
    Sys_Close(windows->length-1);
}

/**
 * Return KCODE
 **/
void destroy(uint32_t windowId) {
    Window* window = (Window*) vector_get(windows, windowId);
    if (window->getOwner() != Sys_GetProcess()) { Sys_Close(KFAIL); }
    window->destroy();
    free(window);
    vector_set(windows, windowId, NULL);
    Sys_Close(KSUCCESS);
}

/**
 * Return Framebuffer
 **/
void getFramebuffer(uint32_t windowId) {
    Window* window = (Window*) vector_get(windows, windowId);
    if (window->getOwner() != Sys_GetProcess()) { Sys_Close(KFAIL); }
    Sys_Close(window->getFramebufferKey());
}

/**
 * Return KCODE
 **/
void show(uint32_t windowId) {
    Window* window = (Window*) vector_get(windows, windowId);
    if (window->getOwner() != Sys_GetProcess()) { Sys_Close(KFAIL); }
    window->show(true);
    Sys_Close(KSUCCESS);
}

void resize(uint32_t windowId, uint32_t width, uint32_t height) {
    Window* window = (Window*) vector_get(windows, windowId);
    if (window->getOwner() != Sys_GetProcess()) { Sys_Close(KFAIL); }
    window->resize(width, height);
    Sys_Close(window->getFramebufferKey());
}

void move(uint32_t windowId, uint32_t x, uint32_t y) {
    Window* window = (Window*) vector_get(windows, windowId);
    if (window->getOwner() != Sys_GetProcess()) { Sys_Close(KFAIL); }
    window->move(x, y);
    Sys_Close(KSUCCESS);
}

/**
 * Return KCODE
 **/
void hide(uint32_t windowId) {
    Window* window = (Window*) vector_get(windows, windowId);
    if (window->getOwner() != Sys_GetProcess()) { Sys_Close(KFAIL); }
    window->show(false);
    Sys_Close(KSUCCESS);
}

/**
 * Return Framebuffer
 **/
void getHeight(uint32_t windowId) {
    Window* window = (Window*) vector_get(windows, windowId);
    Sys_Close(window->getHeight());
}

/**
 * Return Framebuffer
 **/
void getWidth(uint32_t windowId) {
    Window* window = (Window*) vector_get(windows, windowId);
    Sys_Close(window->getWidth());
}



void initUISD() {

    Sys_Createthread(self, (uintptr_t) &create, PriviledgeApp, &createThread);
    Sys_Createthread(self, (uintptr_t) &destroy, PriviledgeApp, &destroyThread);
    Sys_Createthread(self, (uintptr_t) &getFramebuffer, PriviledgeApp, &getFramebufferThread);
    Sys_Createthread(self, (uintptr_t) &getWidth, PriviledgeApp, &getWidthThread);
    Sys_Createthread(self, (uintptr_t) &getHeight, PriviledgeApp, &getHeightThread);
    Sys_Createthread(self, (uintptr_t) &show, PriviledgeApp, &showThread);
    Sys_Createthread(self, (uintptr_t) &hide, PriviledgeApp, &hideThread);
    Sys_Createthread(self, (uintptr_t) &resize, PriviledgeApp, &resizeThread);
    Sys_Createthread(self, (uintptr_t) &move, PriviledgeApp, &moveThread);

    uintptr_t address = getFreeAlignedSpace(sizeof(uisd_graphics_t));
    ksmem_t key = NULL;
    Sys_CreateMemoryField(self, sizeof(uisd_graphics_t), &address, &key, MemoryFieldTypeShareSpaceRO);
    uisd_graphics_t* OrbSrv = (uisd_graphics_t*) address;
    OrbSrv->ControllerHeader.Process = ShareProcessKey(self);
    OrbSrv->ControllerHeader.IsReadWrite = false;
    OrbSrv->ControllerHeader.Version = Orb_Srv_Version;
    OrbSrv->ControllerHeader.VendorID = Kot_VendorID;
    OrbSrv->ControllerHeader.Type = ControllerTypeEnum_Graphics;

    OrbSrv->create = MakeShareableThread(createThread, PriviledgeApp);
    OrbSrv->destroy = MakeShareableThread(destroyThread, PriviledgeApp);
    OrbSrv->getFramebuffer = MakeShareableThread(getFramebufferThread, PriviledgeApp);
    OrbSrv->getWidth = MakeShareableThread(getWidthThread, PriviledgeApp);
    OrbSrv->getHeight = MakeShareableThread(getHeightThread, PriviledgeApp);
    OrbSrv->show = MakeShareableThread(showThread, PriviledgeApp);
    OrbSrv->hide = MakeShareableThread(hideThread, PriviledgeApp);
    OrbSrv->resize = MakeShareableThread(resizeThread, PriviledgeApp);
    OrbSrv->move = MakeShareableThread(moveThread, PriviledgeApp);

    CreateControllerUISD(ControllerTypeEnum_Graphics, key, true);

}

thread_t MouseRelativeInterrupt;

void CursorInterrupt(int64_t x, int64_t y, int64_t z, uint64_t status){
    printf("Mouse from orb %x %x %x %x", x, y, z, status);
    Sys_Event_Close();
}

void initCursor() {
    Sys_Createthread(Sys_GetProcess(), (uintptr_t)&CursorInterrupt, PriviledgeApp, &MouseRelativeInterrupt);
    BindMouseRelative(MouseRelativeInterrupt, false);
}

void initOrb() {

    self = ShareProcessKey(Sys_GetProcess());

    monitors = vector_create();
    windows = vector_create();

    srv_system_callback_t* callback = Srv_System_GetFrameBuffer(true);
    srv_system_framebuffer_t* bootframebuffer = (srv_system_framebuffer_t*)callback->Data;
    free(callback);

    size64_t fb_size = bootframebuffer->Pitch * bootframebuffer->Height;
    
    uint64_t virtualAddress = (uint64_t)MapPhysical((uintptr_t)bootframebuffer->Address, fb_size);

    Monitor* monitor0 = new Monitor(self, (uintptr_t) virtualAddress, bootframebuffer->Width, bootframebuffer->Height, 0, 0);

    free(bootframebuffer);
    
    vector_push(monitors, monitor0);

    loadBootGraphics(monitor0->getBackground()->getFramebuffer());

    initCursor();

    Sys_Createthread(self, (uintptr_t) &threadRender, PriviledgeDriver, &renderThread);
    Sys_Execthread(renderThread, NULL, ExecutionTypeQueu, NULL);

}

extern "C" int main() {

    initOrb();
    initUISD();

    Printlog("[ORB] Service started");

    return KSUCCESS;

}