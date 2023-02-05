#include <render/render.h>

vector_t* Monitors = NULL;

windowc* FirstWindowNode;

uint64_t RenderMutex;

thread_t RenderThread = NULL;
bool IsRendering = false;

void RenderWindows(){
    // todo: multi threads monitor rendering
    for(uint64_t i = 0; i < Monitors->length; i++){
        monitorc* Monitor = (monitorc*)vector_get(Monitors, i);
        Monitor->Update(FirstWindowNode);
    }
}

void UpdateAllEvents(){
    for(uint64_t i = 0; i < Monitors->length; i++){
        monitorc* Monitor = (monitorc*)vector_get(Monitors, i);
        Monitor->UpdateEvents(FirstWindowNode);
    }
}


void ThreadRender(){
    while(IsRendering){
        RenderWindows();
    }
    Sys_Close(KSUCCESS);
}

KResult StartRender(){
    IsRendering = true;
    return Sys_Execthread(RenderThread, NULL, ExecutionTypeQueu, NULL);
}

KResult StopRender(){
    if(IsRendering){
        IsRendering = false;
        return KSUCCESS;
    }
    return KFAIL;
}

KResult InitializeRender(){
    Monitors = vector_create();
    return Sys_Createthread(ShareableProcess, (uintptr_t) &ThreadRender, PriviledgeDriver, NULL, &RenderThread);
}