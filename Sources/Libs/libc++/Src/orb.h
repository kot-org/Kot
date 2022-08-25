#pragma once

#include <kot/uisd.h>
#include <kot/utils.h>
#include <kot/types.h>

namespace orb {

    uisd_graphics_t* OrbSrv = NULL; 

    uint64_t CreateWindow(uint64_t width, uint64_t height) {
        if (OrbSrv == NULL) {
            uintptr_t addressReceived = getFreeAlignedSpace(sizeof(uisd_graphics_t));
            GetControllerUISD(ControllerTypeEnum_Graphics, &addressReceived, true);
            OrbSrv = (uisd_graphics_t*) addressReceived;
        }
        arguments_t* arguments = (arguments_t*) malloc(sizeof(arguments_t));
        arguments->arg[0] = width;
        arguments->arg[1] = height;
        ksmem_t result = Sys_Execthread(OrbSrv->CreateWindow, arguments, ExecutionTypeQueuAwait, NULL);
        return result;
    }

    void DestroyWindow(uint64_t wid) {
        /* TODO */
    }

    uintptr_t GetWindowbuffer(uint64_t wid) {
        /* TODO */
        return NULL;
    } 

    void HideWindow(uint64_t wid) {
        /* TODO */
    }

    void ShowWindow(uint64_t wid) {
        /* TODO */
    }

    void SetWindowWidth(uint64_t wid) {
        
    }

    void SetWindowHeight(uint64_t wid) {

    }

    uint64_t GetWindowWidth(uint64_t wid) {
        /* TODO */
        return NULL;
    }

    uint64_t GetWindowHeight(uint64_t wid) {
        /* TODO */
        return NULL;
    }

}