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

uint32_t Create(uint32_t width, uint32_t height, int32_t xPos, int32_t yPos) {
    if (OrbSrv == NULL) { GetOrbSrv(); }
    arguments_t arguments;
    arguments.arg[0] = width;
    arguments.arg[1] = height;
    arguments.arg[2] = xPos;
    arguments.arg[3] = yPos;
    ksmem_t result = Sys_Execthread(OrbSrv->CreateWindow, &arguments, ExecutionTypeQueuAwait, NULL); 
    return result;
}

void Destroy(uint32_t wid) {
    if (OrbSrv == NULL) { GetOrbSrv(); }
    arguments_t arguments;
    arguments.arg[0] = wid;
    Sys_Execthread(OrbSrv->DestroyWindow, &arguments, ExecutionTypeQueuAwait, NULL);
}

uint32_t GetWidth(uint32_t wid) {
    if (OrbSrv == NULL) { GetOrbSrv(); }
    arguments_t arguments;
    arguments.arg[0] = wid;
    ksmem_t result = Sys_Execthread(OrbSrv->GetWidth, &arguments, ExecutionTypeQueuAwait, NULL);
    return result;
}

uint32_t GetHeight(uint32_t wid) {
    if (OrbSrv == NULL) { GetOrbSrv(); }
    arguments_t arguments;
    arguments.arg[0] = wid;
    ksmem_t result = Sys_Execthread(OrbSrv->GetHeight, &arguments, ExecutionTypeQueuAwait, NULL);
    return result;
}

framebuffer_t* GetFramebuffer(uint32_t wid) {
    if (OrbSrv == NULL) { GetOrbSrv(); }
    framebuffer_t* fb = (framebuffer_t*) calloc(sizeof(framebuffer_t));
    fb->height = GetHeight(wid);
    fb->width = GetWidth(wid);
    fb->pitch = fb->width * 4;
    fb->size = fb->pitch * fb->height;
    arguments_t arguments;
    arguments.arg[0] = wid;
    ksmem_t MemoryShare = Sys_Execthread(OrbSrv->GetFramebuffer, &arguments, ExecutionTypeQueuAwait, NULL);
    uintptr_t addressReceiveShare = getFreeAlignedSpace(fb->size);
    process_t proc = Sys_GetProcess();
    Sys_AcceptMemoryField(proc, MemoryShare, &addressReceiveShare);
    fb->addr = addressReceiveShare;
    return fb;
} 

void Hide(uint32_t wid) {
    if (OrbSrv == NULL) { GetOrbSrv(); }
    arguments_t arguments;
    arguments.arg[0] = wid;
    Sys_Execthread(OrbSrv->Hide, &arguments, ExecutionTypeQueuAwait, NULL);
}

void Show(uint32_t wid) {
    if (OrbSrv == NULL) { GetOrbSrv(); }
    arguments_t arguments;
    arguments.arg[0] = wid;
    Sys_Execthread(OrbSrv->Show, &arguments, ExecutionTypeQueuAwait, NULL);
}

void Resize(uint32_t wid, uint32_t width, uint32_t height) {
    if (OrbSrv == NULL) { GetOrbSrv(); }
    
}