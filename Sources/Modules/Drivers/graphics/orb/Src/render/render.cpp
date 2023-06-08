#include <render/render.h>

void ThreadRenderEntry(){
    renderc* Render = (renderc*)kot_Sys_GetExternalDataThread();
    Render->ThreadRender();
    kot_Sys_Close(KSUCCESS);
}

renderc::renderc(orbc* Parent){
    Monitors = kot_vector_create();
    FirstWindowNode = NULL;
    kot_Sys_CreateThread(ShareableProcess, (void*) &ThreadRenderEntry, PriviledgeDriver, (uint64_t)this, &RenderThread);
}

void renderc::RenderWindows(){
    // todo: multi threads monitor rendering
    atomicAcquire(&RenderLock, 0);
    for(uint64_t i = 0; i < Monitors->length; i++){
        monitorc* Monitor = (monitorc*)kot_vector_get(Monitors, i);
        Monitor->Update(FirstWindowNode);
    }
    atomicUnlock(&RenderLock, 0);
}

void renderc::UpdateAllEvents(){
    atomicAcquire(&RenderLock, 0);
    for(uint64_t i = 0; i < Monitors->length; i++){
        monitorc* Monitor = (monitorc*)kot_vector_get(Monitors, i);
        Monitor->UpdateEvents(FirstWindowNode);
    }
    atomicUnlock(&RenderLock, 0);
}


void renderc::ThreadRender(){
    while(IsRendering){
        RenderWindows();
    }
}

KResult renderc::StartRender(){
    IsRendering = true;
    return kot_Sys_ExecThread(RenderThread, NULL, ExecutionTypeQueu, NULL);
}

KResult renderc::StopRender(){
    if(IsRendering){
        IsRendering = false;
        return KSUCCESS;
    }
    return KFAIL;
}

KResult renderc::AddMonitor(monitorc* Monitor){
    kot_vector_push(Monitors, Monitor);
    return KSUCCESS;
}

KResult renderc::RemoveMonitor(){
    // TODO
    return KSUCCESS;
}