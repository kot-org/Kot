#include <core/main.h>

using namespace std;

process_t ShareableProcess;

void InitialiseOrb(){
    ShareableProcess = ShareProcessKey(Sys_GetProcess());
    InitializeRender();

    srv_system_callback_t* callback = Srv_System_GetFramebuffer(true);
    srv_system_framebuffer_t* bootframebuffer = (srv_system_framebuffer_t*)callback->Data;
    free(callback);

    size64_t fb_size = bootframebuffer->Pitch * bootframebuffer->Height;

    uint64_t virtualAddress = (uint64_t)MapPhysical((uintptr_t)bootframebuffer->Address, fb_size);

    monitorc* monitor0 = new monitorc(ShareableProcess, (uintptr_t) virtualAddress, bootframebuffer->Width, bootframebuffer->Height, bootframebuffer->Pitch, bootframebuffer->Bpp, 0, 0);

    CursorMaxPosition.x = monitor0->GetWidth()-1;
    CursorMaxPosition.y = monitor0->GetHeight()-1;

    free(bootframebuffer);

    vector_push(Monitors, monitor0);
    
    windowc* LoadingScreen = new windowc(Window_Type_Background, NULL);
    LoadingScreen->Resize(Window_Max_Size, Window_Max_Size);
    LoadingScreen->SetVisible(true);
    
    LoadBootGraphics(LoadingScreen->GetFramebuffer());
    
    InitializeCursor();

}

extern "C" int main() {
    InitialiseOrb();
    InitialiseServer();

    StartRender();
    Printlog("[GRAPHICS/ORB] Service started");
    
    return KSUCCESS;
}