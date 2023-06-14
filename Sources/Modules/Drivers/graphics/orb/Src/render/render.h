#pragma once

#include <core/main.h>

class orbc;
class windowc;
class monitorc;
class desktopc;
class renderc;

class renderc{
    public:
        renderc(orbc* Parent);
        void RenderWindows();
        void UpdateAllEvents();
        void ThreadRender();
        KResult StartRender();
        KResult StopRender();
        KResult AddMonitor(class monitorc* Monitor);
        KResult RemoveMonitor();

        kot_vector_t* Monitors = NULL;
        windowc* FirstWindowNode;
        kot_thread_t RenderThread = NULL;
        bool IsRendering = false;   
};