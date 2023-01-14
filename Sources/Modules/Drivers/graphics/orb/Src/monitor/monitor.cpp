#include <monitor/monitor.h>

monitorc::monitorc(process_t orb, uintptr_t fb_addr, uint64_t Width, uint64_t Height, uint64_t Pitch, uint64_t Bpp, uint32_t XPosition, uint32_t YPosition){
    this->XPosition = XPosition;
    this->XMaxPosition = XPosition + Width;

    this->YPosition = YPosition;
    this->YMaxPosition = YPosition + Height;

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

void dynamicBlit(framebuffer_t* to, framebuffer_t* from, uint32_t x, uint32_t y, uint32_t monitorXoffset, uint32_t monitorYoffset) {
    BlitFramebuffer(to, from, x, y);
}

void monitorc::Update(vector_t* Background, vector_t* Windows, vector_t* Foreground){
    // Background
    for(uint64_t i = 0; i < Background->length; i++){
        windowc* window = (windowc*) vector_get(Background, i);
        dynamicBlit(this->BackFramebuffer, window->GetFramebuffer(), window->GetX(), window->GetY(), this->XPosition, this->YPosition);
    }

    // Windows
    for(uint64_t i = 0; i < Windows->length; i++){
        windowc* window = (windowc*) vector_get(Windows, i);
        dynamicBlit(this->BackFramebuffer, window->GetFramebuffer(), window->GetX(), window->GetY(), this->XPosition, this->YPosition);
    }

    // Foreground
    for(uint64_t i = Foreground->length; i != 0; i--){
        windowc* window = (windowc*) vector_get(Foreground, i - 1);
        dynamicBlit(this->BackFramebuffer, window->GetFramebuffer(), window->GetX(), window->GetY(), this->XPosition, this->YPosition);
    }

    DrawCursor(this->BackFramebuffer, BitmapMask, PixelMap);

    memcpy(this->MainFramebuffer->Buffer, this->BackFramebuffer->Buffer, this->MainFramebuffer->Size);
}