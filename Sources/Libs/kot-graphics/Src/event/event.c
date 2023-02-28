#include <kot-graphics/event.h>

graphiceventbuffer_t* CreateEventBuffer(uint64_t Width, uint64_t Height){
    graphiceventbuffer_t* EventBuffer = malloc(sizeof(graphiceventbuffer_t));
    
    EventBuffer->Width = Width;
    EventBuffer->Height = Height;

    EventBuffer->Bpp = sizeof(uint64_t) * 8;
    EventBuffer->Btpp = sizeof(uint64_t);

    EventBuffer->Pitch = EventBuffer->Width * EventBuffer->Btpp;
    EventBuffer->Size = EventBuffer->Pitch * EventBuffer->Height;
    EventBuffer->Buffer = calloc(EventBuffer->Size);

    return EventBuffer;
}

void SetGraphicEventbuffer(graphiceventbuffer_t* Framebuffer, uint64_t Value, uint64_t Width, uint64_t Height, uint64_t PositionX, uint64_t PositionY){
    uint64_t Buffer = (uint64_t)Framebuffer->Buffer;

    Buffer += PositionX * Framebuffer->Btpp + PositionY * Framebuffer->Pitch; // offset

    uint64_t WithCopy = Width;

    if(PositionX + WithCopy >= Framebuffer->Width){
        WithCopy = Framebuffer->Width - PositionX;
    }

    uint64_t HeightCopy = Height;

    if(PositionY + HeightCopy >= Framebuffer->Height){
        HeightCopy = Framebuffer->Height - PositionY;
    }

    uint64_t PitchCopy = WithCopy * Framebuffer->Btpp;

    for(uint64_t H = 0; H < HeightCopy; H++){
        memset64((uintptr_t)Buffer, Value, PitchCopy);
        Buffer += Framebuffer->Pitch;
    }
}

void SetGraphicEventbufferRadius(graphiceventbuffer_t* Framebuffer, uint64_t Value, uint64_t Width, uint64_t Height, uint64_t PositionX, uint64_t PositionY, uint64_t BorderRadius){
    uint64_t ToBuffer = (uint64_t) Framebuffer->Buffer;

    ToBuffer += PositionX * Framebuffer->Btpp + PositionY * Framebuffer->Pitch;

    if (PositionX + Width >= Framebuffer->Width) {
        Width = Framebuffer->Width - PositionX;
    }

    if (PositionY + Height >= Framebuffer->Height) {
        Height = Framebuffer->Height - PositionY;
    }

    uint64_t PitchCopy = Width * Framebuffer->Btpp;

    uint64_t Ray = BorderRadius / 2;
    for (uint64_t h = 0; h < Ray && h < Height && h < Height; h++) {
        uint64_t CircleH = h;
        uint64_t Height = (Ray - CircleH);
        uint64_t LeftOffset = (uint64_t)(Ray - sqrt(Ray*Ray-Height*Height)) * Framebuffer->Btpp;
        memset64((uintptr_t) (ToBuffer + LeftOffset), Value, PitchCopy - (LeftOffset * 2));
        ToBuffer += Framebuffer->Pitch;
    }

    for (uint64_t h = Ray; h < Height - Ray && h < Height; h++) {
        memset64((uintptr_t) ToBuffer, Value, PitchCopy);
        ToBuffer += Framebuffer->Pitch;
    }

    for (uint64_t h = Height - Ray; h < Height; h++) {
        uint64_t CircleH = Height - h;
        uint64_t Height = (Ray - CircleH);
        uint64_t LeftOffset = (uint64_t)(Ray - sqrt(Ray*Ray-Height*Height)) * Framebuffer->Btpp;
        memset64((uintptr_t) (ToBuffer + LeftOffset), Value, PitchCopy - (LeftOffset * 2));
        ToBuffer += Framebuffer->Pitch;
    }
}

void BlitGraphicEventbuffer(graphiceventbuffer_t* To, graphiceventbuffer_t* From, uint64_t PositionX, uint64_t PositionY){
    uint64_t ToBuffer = (uint64_t)To->Buffer;
    uint64_t FromBuffer = (uint64_t)From->Buffer;

    ToBuffer += PositionX * To->Btpp + PositionY * To->Pitch; // offset

    uint64_t WithCopy = From->Width;

    if(PositionX + WithCopy >= To->Width){
        WithCopy = To->Width - PositionX;
    }

    uint64_t HeightCopy = From->Height;

    if(PositionY + HeightCopy >= To->Height){
        HeightCopy = To->Height - PositionY;
    }

    uint64_t PitchCopy = WithCopy * To->Btpp;

    for(uint64_t H = 0; H < HeightCopy; H++){
        memcpy((uintptr_t)ToBuffer, (uintptr_t)FromBuffer, PitchCopy);
        ToBuffer += To->Pitch;
        FromBuffer += From->Pitch;
    }
}


void BlitGraphicEventbufferRadius(graphiceventbuffer_t* to, graphiceventbuffer_t* from, uint64_t PositionX, uint64_t PositionY, uint64_t BorderRadius){
    uint64_t ToBuffer = (uint64_t) to->Buffer;
    uint64_t FromBuffer = (uint64_t) from->Buffer;

    ToBuffer += PositionX * to->Btpp + PositionY * to->Pitch;
    uint64_t WidthCopy = from->Width;

    if (PositionX + WidthCopy >= to->Width) {
        WidthCopy = to->Width - PositionX;
    }

    uint64_t HeightCopy = from->Height;

    if (PositionY + HeightCopy >= to->Height) {
        HeightCopy = to->Height - PositionY;
    }

    uint64_t PitchCopy = WidthCopy * to->Btpp;

    uint64_t Ray = BorderRadius / 2;
    for (uint64_t h = 0; h < Ray && h < HeightCopy; h++) {
        uint64_t CircleH = h;
        uint64_t Height = (Ray - CircleH);
        uint64_t LeftOffset = (uint64_t)(Ray - sqrt(Ray*Ray-Height*Height)) * to->Btpp;
        if (LeftOffset < 0) {
            LeftOffset = 0;
        }
        memcpy((uintptr_t) (ToBuffer + LeftOffset), (uintptr_t) (FromBuffer + LeftOffset), PitchCopy - (LeftOffset * 2));
        ToBuffer += to->Pitch;
        FromBuffer += from->Pitch;
    }

    for (uint64_t h = Ray; h < HeightCopy - Ray && h < HeightCopy; h++) {
        memcpy((uintptr_t) ToBuffer, (uintptr_t) FromBuffer, PitchCopy);
        ToBuffer += to->Pitch;
        FromBuffer += from->Pitch;
    }

    for (uint64_t h = HeightCopy - Ray; h < HeightCopy; h++) {
        uint64_t CircleH = HeightCopy - h;
        uint64_t Height = (Ray - CircleH);
        uint64_t LeftOffset = (uint64_t)(Ray - sqrt(Ray*Ray-Height*Height)) * to->Btpp;
        if (LeftOffset < 0) {
            LeftOffset = 0;
        }
        memcpy((uintptr_t) (ToBuffer + LeftOffset), (uintptr_t) (FromBuffer + LeftOffset), PitchCopy - (LeftOffset * 2));
        ToBuffer += to->Pitch;
        FromBuffer += from->Pitch;
    }
}

uint64_t GetEventData(graphiceventbuffer_t* EventBuffer, uint64_t PositionX, uint64_t PositionY){
    uint64_t Index = PositionX * EventBuffer->Btpp + PositionY * EventBuffer->Pitch;
    return *(uint64_t*)((uint64_t)EventBuffer->Buffer + Index);
}