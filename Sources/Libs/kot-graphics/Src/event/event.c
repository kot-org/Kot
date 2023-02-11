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

uint64_t GetEventData(graphiceventbuffer_t* EventBuffer, uint64_t PositionX, uint64_t PositionY){
    uint64_t Index = PositionX * EventBuffer->Btpp + PositionY * EventBuffer->Pitch;
    return *(uint64_t*)((uint64_t)EventBuffer->Buffer + Index);
}