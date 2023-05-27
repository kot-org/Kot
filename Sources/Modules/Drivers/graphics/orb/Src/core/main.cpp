#include <core/main.h>

using namespace std;

kot_process_t ShareableProcess;

orbc::orbc(){
    ShareableProcess = kot_ShareProcessKey(kot_Sys_GetProcess());

    Render = new renderc(this);
    Hid = new hidc(this);
    Desktop = new desktopc(this);

    kot_srv_system_callback_t* callback = kot_Srv_System_GetFramebuffer(true);
    kot_srv_system_framebuffer_t* bootframebuffer = (kot_srv_system_framebuffer_t*)callback->Data;
    free(callback);

    size64_t FbSize = bootframebuffer->Pitch * bootframebuffer->Height;

    uint64_t virtualAddress = (uint64_t)kot_MapPhysical((void*)bootframebuffer->Address, FbSize);

    
    monitorc* monitor0 = new monitorc(this, (void*)virtualAddress, bootframebuffer->Width, bootframebuffer->Height, bootframebuffer->Pitch, bootframebuffer->Bpp, 0, 0);

    Hid->CursorMaxPosition.x = monitor0->GetWidth()-1;
    Hid->CursorMaxPosition.y = monitor0->GetHeight()-1;

    free(bootframebuffer);
}

extern "C" int main() {
    orbc* Orb = new orbc();
    InitialiseServer(Orb);

    Orb->Render->StartRender();
    kot_Printlog("[GRAPHICS/ORB] Service started");
    
    return KSUCCESS;
}