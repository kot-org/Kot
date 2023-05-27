#include <monitor/monitor.h>

monitorc::monitorc(orbc* Parent, void* FbBase, uint64_t Width, uint64_t Height, uint64_t Pitch, uint64_t Bpp, uint32_t XPosition, uint32_t YPosition){
    this->Orb = Parent;
    
    this->XPosition = XPosition;
    this->XMaxPosition = XPosition + Width;

    this->YPosition = YPosition;
    this->YMaxPosition = YPosition + Height;

    MainFramebuffer = (kot_framebuffer_t*)calloc(1, sizeof(kot_framebuffer_t));
    BackFramebuffer = (kot_framebuffer_t*)calloc(1, sizeof(kot_framebuffer_t));

    MainFramebuffer->Buffer = FbBase;
    MainFramebuffer->Width = Width;
    MainFramebuffer->Height = Height;
    MainFramebuffer->Pitch = Pitch;
    MainFramebuffer->Bpp = Bpp;
    MainFramebuffer->Btpp = Bpp / 8;
    MainFramebuffer->Size = MainFramebuffer->Pitch * Height;

    BackFramebuffer->Buffer = calloc(Height, MainFramebuffer->Pitch);
    BackFramebuffer->Width = Width;
    BackFramebuffer->Height = Height;
    BackFramebuffer->Pitch = MainFramebuffer->Pitch;
    BackFramebuffer->Bpp = MainFramebuffer->Bpp;
    BackFramebuffer->Btpp = MainFramebuffer->Btpp;
    BackFramebuffer->Size = MainFramebuffer->Size;

    Eventbuffer = CreateEventBuffer(Width, Height);

    Orb->Render->AddMonitor(this);
    Orb->Desktop->AddMonitor(this);

    UpdateEvents(Orb->Render->FirstWindowNode);
}

uint64_t monitorc::GetWidth() {
    return this->MainFramebuffer->Width;
}

uint64_t monitorc::GetHeight() {
    return this->MainFramebuffer->Height;
}

void monitorc::Move(uint64_t XPosition, uint64_t YPosition) {
    this->XPosition = XPosition;
    this->YPosition = YPosition;
}

void DynamicBlit(kot_framebuffer_t* to, kot_framebuffer_t* from, uint64_t x, uint64_t y, uint64_t MonitorXoffset, uint64_t MonitorYoffset) {
    BlitFramebuffer(to, from, x, y);
}

void monitorc::UpdateEvents(windowc* FirstWindowNode){
    windowc* Window = FirstWindowNode;

    Orb->Desktop->UpdateBackgroundEvent(this);

    while(Window){
        BlitGraphicEventbuffer(this->Eventbuffer, Window->GetEventbuffer(), Window->GetX(), Window->GetY());
        Window = Window->Next;
    }

    Orb->Desktop->UpdateWidgetsEvent(this);
}

void monitorc::Update(windowc* FirstWindowNode){
    windowc* Window = FirstWindowNode;

    Orb->Desktop->UpdateBackground(this);

    while(Window){
        if(atomicLock(&Window->Lock, 0)){
            DynamicBlit(this->BackFramebuffer, Window->GetFramebuffer(), Window->GetX(), Window->GetY(), this->XPosition, this->YPosition);
            atomicUnlock(&Window->Lock, 0);
        }
        Window = Window->Next;
    }

    Orb->Desktop->UpdateWidgets(this);

    Orb->Hid->DrawCursor(this->BackFramebuffer);

    memcpy(this->MainFramebuffer->Buffer, this->BackFramebuffer->Buffer, this->MainFramebuffer->Size);
}