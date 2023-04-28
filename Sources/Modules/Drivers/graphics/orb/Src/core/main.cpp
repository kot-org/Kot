#include <core/main.h>

using namespace std;

process_t ShareableProcess;

orbc::orbc(){
    ShareableProcess = ShareProcessKey(Sys_GetProcess());

    Render = new renderc(this);
    Hid = new hidc(this);
    Desktop = new desktopc(this);

    srv_system_callback_t* callback = Srv_System_GetFramebuffer(true);
    srv_system_framebuffer_t* bootframebuffer = (srv_system_framebuffer_t*)callback->Data;
    free(callback);

    size64_t FbSize = bootframebuffer->Pitch * bootframebuffer->Height;

    uint64_t virtualAddress = (uint64_t)MapPhysical((uintptr_t)bootframebuffer->Address, FbSize);

    
    monitorc* monitor0 = new monitorc(this, (uintptr_t)virtualAddress, bootframebuffer->Width, bootframebuffer->Height, bootframebuffer->Pitch, bootframebuffer->Bpp, 0, 0);

    Hid->CursorMaxPosition.x = monitor0->GetWidth()-1;
    Hid->CursorMaxPosition.y = monitor0->GetHeight()-1;

    free(bootframebuffer);
}

extern "C" int main() {
    orbc* Orb = new orbc();
    InitialiseServer(Orb);

    Orb->Render->StartRender();
    Printlog("[GRAPHICS/ORB] Service started");
    
    return KSUCCESS;
}