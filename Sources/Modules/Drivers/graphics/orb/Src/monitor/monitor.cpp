#include <monitor/monitor.h>

monitorc::monitorc(process_t orb, uintptr_t fb_addr, uint64_t Width, uint64_t Height, uint64_t Pitch, uint64_t Bpp, uint32_t XPosition, uint32_t YPosition){
    this->XPosition = XPosition;
    this->XPositionWithDock = XPosition;
    this->XMaxPosition = XPosition + Width;
    this->XMaxPositionWithDock = XPosition + Width;

    this->YPosition = YPosition;
    this->YPositionWithDock = YPosition;
    this->YMaxPosition = YPosition + Height;
    this->YMaxPositionWithDock = YPosition + Height;

    MainFramebuffer = (framebuffer_t*)calloc(sizeof(framebuffer_t));
    BackFramebuffer = (framebuffer_t*)calloc(sizeof(framebuffer_t));

    MainFramebuffer->Buffer = fb_addr;
    MainFramebuffer->Width = Width;
    MainFramebuffer->Height = Height;
    MainFramebuffer->Pitch = Pitch;
    MainFramebuffer->Bpp = Bpp;
    MainFramebuffer->Btpp = Bpp / 8;
    MainFramebuffer->Size = MainFramebuffer->Pitch * Height;

    BackFramebuffer->Buffer = calloc(MainFramebuffer->Pitch * Height);
    BackFramebuffer->Width = Width;
    BackFramebuffer->Height = Height;
    BackFramebuffer->Pitch = MainFramebuffer->Pitch;
    BackFramebuffer->Bpp = MainFramebuffer->Bpp;
    BackFramebuffer->Btpp = MainFramebuffer->Btpp;
    BackFramebuffer->Size = MainFramebuffer->Size;

    Eventbuffer = CreateEventBuffer(Width, Height);
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

void DynamicBlit(framebuffer_t* to, framebuffer_t* from, uint64_t x, uint64_t y, uint64_t MonitorXoffset, uint64_t MonitorYoffset) {
    BlitFramebuffer(to, from, x, y);
}

void monitorc::UpdateEvents(windowc* FirstWindowNode){
    windowc* Window = FirstWindowNode;

    while(Window){
        BlitGraphicEventbuffer(this->Eventbuffer, Window->GetEventbuffer(), Window->GetX(), Window->GetY());
        Window = Window->Next;
    }
}

void monitorc::Update(windowc* FirstWindowNode){
    windowc* Window = FirstWindowNode;

    while(Window){
        DynamicBlit(this->BackFramebuffer, Window->GetFramebuffer(), Window->GetX(), Window->GetY(), this->XPosition, this->YPosition);
        Window = Window->Next;
    }

    DrawCursor(this->BackFramebuffer, BitmapMask, PixelMap);

    memcpy(this->MainFramebuffer->Buffer, this->BackFramebuffer->Buffer, this->MainFramebuffer->Size);
}