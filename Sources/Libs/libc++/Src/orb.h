#pragma once

#include <kot/uisd.h>
#include <kot/utils.h>
#include <kot/types.h>

namespace orb {

    size64_t fb_size(uint32_t width, uint32_t height) {
        return width * 4 * height;
    }

    uisd_graphics_t* OrbSrv = NULL; 

    void GetOrbSrv() {
        uintptr_t addressReceived = getFreeAlignedSpace(sizeof(uisd_graphics_t));
        GetControllerUISD(ControllerTypeEnum_Graphics, &addressReceived, true);
        OrbSrv = (uisd_graphics_t*) addressReceived;
    }

    uint32_t Create(uint32_t width, uint32_t height, int32_t xPos, int32_t yPos) {
        if (OrbSrv == NULL) { GetOrbSrv(); }
        arguments_t* arguments = (arguments_t*) malloc(sizeof(arguments_t));
        arguments->arg[0] = width;
        arguments->arg[1] = height;
        arguments->arg[2] = xPos;
        arguments->arg[3] = yPos;
        ksmem_t result = Sys_Execthread(OrbSrv->CreateWindow, arguments, ExecutionTypeQueuAwait, NULL);
        return result;
    }

    void Destroy(uint32_t wid) {
        if (OrbSrv == NULL) { GetOrbSrv(); }
        /* TODO */
    }

    uint32_t GetWidth(uint32_t wid) {
        if (OrbSrv == NULL) { GetOrbSrv(); }
        arguments_t* arguments = (arguments_t*) malloc(sizeof(arguments_t));
        arguments->arg[0] = wid;
        ksmem_t result = Sys_Execthread(OrbSrv->GetWidth, arguments, ExecutionTypeQueuAwait, NULL);
        return result;
    }

    uint32_t GetHeight(uint32_t wid) {
        if (OrbSrv == NULL) { GetOrbSrv(); }
        arguments_t* arguments = (arguments_t*) malloc(sizeof(arguments_t));
        arguments->arg[0] = wid;
        ksmem_t result = Sys_Execthread(OrbSrv->GetHeight, arguments, ExecutionTypeQueuAwait, NULL);
        return result;
    }

    uintptr_t GetFramebuffer(uint32_t wid) {
        if (OrbSrv == NULL) { GetOrbSrv(); }
        arguments_t* arguments = (arguments_t*) malloc(sizeof(arguments_t));
        arguments->arg[0] = wid;
        ksmem_t MemoryShare = Sys_Execthread(OrbSrv->GetFramebuffer, arguments, ExecutionTypeQueuAwait, NULL);
        uintptr_t addressReceiveShare = getFreeAlignedSpace(fb_size(GetWidth(wid), GetHeight(wid)));
        process_t proc = Sys_GetProcess();
        Sys_AcceptMemoryField(proc, MemoryShare, &addressReceiveShare);
        return addressReceiveShare;
    } 

    void Hide(uint32_t wid) {
        if (OrbSrv == NULL) { GetOrbSrv(); }
        /* TODO */
    }

    void Show(uint32_t wid) {
        if (OrbSrv == NULL) { GetOrbSrv(); }
        /* TODO */
    }

    void Resize(uint32_t wid, uint32_t width, uint32_t height) {
        if (OrbSrv == NULL) { GetOrbSrv(); }
        
    }

}