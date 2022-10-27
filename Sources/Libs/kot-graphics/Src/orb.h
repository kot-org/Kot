#pragma once

#include <kot/uisd.h>
#include <kot/utils.h>
#include <kot/types.h>

#include <kot-graphics/utils.h> 

uisd_graphics_t* OrbSrv = NULL;

void GetOrbSrv() {
    uintptr_t addressReceived = getFreeAlignedSpace(sizeof(uisd_graphics_t));
    GetControllerUISD(ControllerTypeEnum_Graphics, &addressReceived, true);
    OrbSrv = (uisd_graphics_t*) addressReceived;
}

uint32_t create(uint32_t width, uint32_t height, int32_t xPos, int32_t yPos) {
    if (OrbSrv == NULL) { GetOrbSrv(); }
    arguments_t arguments;
    arguments.arg[0] = width;
    arguments.arg[1] = height;
    arguments.arg[2] = xPos;
    arguments.arg[3] = yPos;
    ksmem_t result = Sys_Execthread(OrbSrv->create, &arguments, ExecutionTypeQueuAwait, NULL); 
    return result;
}

void destroy(uint32_t wid) {
    if (OrbSrv == NULL) { GetOrbSrv(); }
    arguments_t arguments;
    arguments.arg[0] = wid;
    Sys_Execthread(OrbSrv->destroy, &arguments, ExecutionTypeQueuAwait, NULL);
}

uint32_t getWidth(uint32_t wid) {
    if (OrbSrv == NULL) { GetOrbSrv(); }
    arguments_t arguments;
    arguments.arg[0] = wid;
    ksmem_t result = Sys_Execthread(OrbSrv->getWidth, &arguments, ExecutionTypeQueuAwait, NULL);
    return result;
}

uint32_t getHeight(uint32_t wid) {
    if (OrbSrv == NULL) { GetOrbSrv(); }
    arguments_t arguments;
    arguments.arg[0] = wid;
    ksmem_t result = Sys_Execthread(OrbSrv->getHeight, &arguments, ExecutionTypeQueuAwait, NULL);
    return result;
}

uint32_t getBpp(uint32_t wid) {
    if (OrbSrv == NULL) { GetOrbSrv(); }
    arguments_t arguments;
    arguments.arg[0] = wid;
    ksmem_t result = Sys_Execthread(OrbSrv->getBpp, &arguments, ExecutionTypeQueuAwait, NULL);
    return result;
}

framebuffer_t* getFramebuffer(uint32_t wid) {
    if (OrbSrv == NULL) { GetOrbSrv(); }
    framebuffer_t* fb = (framebuffer_t*) calloc(sizeof(framebuffer_t));
    fb->height = GetHeight(wid);
    fb->width = GetWidth(wid);
    fb->bpp = GetBpp(wid);
    fb->btpp = fb->bpp / 8;
    fb->pitch = fb->width * fb->btpp;
    fb->size = fb->pitch * fb->height;
    arguments_t arguments;
    arguments.arg[0] = wid;
    ksmem_t MemoryShare = Sys_Execthread(OrbSrv->getFramebuffer, &arguments, ExecutionTypeQueuAwait, NULL);
    uintptr_t addressReceiveShare = getFreeAlignedSpace(fb->size);
    process_t proc = Sys_GetProcess();
    Sys_AcceptMemoryField(proc, MemoryShare, &addressReceiveShare);
    fb->addr = addressReceiveShare;
    return fb;
} 

void hide(uint32_t wid) {
    if (OrbSrv == NULL) { GetOrbSrv(); }
    arguments_t arguments;
    arguments.arg[0] = wid;
    Sys_Execthread(OrbSrv->hide, &arguments, ExecutionTypeQueuAwait, NULL);
}

void show(uint32_t wid) {
    if (OrbSrv == NULL) { GetOrbSrv(); }
    arguments_t arguments;
    arguments.arg[0] = wid;
    Sys_Execthread(OrbSrv->show, &arguments, ExecutionTypeQueuAwait, NULL);
}

framebuffer_t* resize(uint32_t wid, uint32_t width, uint32_t height) {
    if (OrbSrv == NULL) { GetOrbSrv(); }
    Graphic::framebuffer_t* fb = (Graphic::framebuffer_t*) calloc(sizeof(Graphic::framebuffer_t));
    fb->width = width;
    fb->height = height;
    fb->pitch = fb->width * 4;
    fb->size = fb->pitch * fb->height;
    arguments_t arguments;
    arguments.arg[0] = wid;
    arguments.arg[1] = width;
    arguments.arg[2] = height;
    ksmem_t MemoryShare = Sys_Execthread(OrbSrv->resize, &arguments, ExecutionTypeQueuAwait, NULL);
    uintptr_t addressReceiveShare = getFreeAlignedSpace(fb->size);
    process_t proc = Sys_GetProcess();
    Sys_AcceptMemoryField(proc, MemoryShare, &addressReceiveShare);
    fb->addr = addressReceiveShare;
    return fb;
    
}

void move(uint32_t wid, uint32_t x, uint32_t y) {
    if (OrbSrv == NULL) { GetOrbSrv(); }
    arguments_t arguments;
    arguments.arg[0] = wid;
    arguments.arg[1] = x;
    arguments.arg[2] = y;
    Sys_Execthread(OrbSrv->move, &arguments, ExecutionTypeQueuAwait, NULL);
}

uint8_t getFocusState(uint32_t wid) {
    if (OrbSrv == NULL) { GetOrbSrv(); }
    arguments_t arguments;
    arguments.arg[0] = wid;
    ksmem_t result = Sys_Execthread(OrbSrv->getFocusState, &arguments, ExecutionTypeQueuAwait, NULL);
    return result;
}