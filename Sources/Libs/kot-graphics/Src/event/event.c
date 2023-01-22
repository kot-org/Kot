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

void BlitGraphicEventbuffer(graphiceventbuffer_t* To, graphiceventbuffer_t* From, uint64_t PositionX, uint64_t PositionY){
    uint64_t to_addr = (uint64_t)To->Buffer;
    uint64_t from_addr = (uint64_t)From->Buffer;

    to_addr += PositionX * To->Btpp + PositionY * To->Pitch; // offset

    uint64_t num;

    if(To->Pitch < From->Pitch){
        num = To->Pitch;
    }else{
        num = From->Pitch;
    } 

    for (uint64_t h = 0; h < From->Height && h + PositionY < To->Height; h++) {
        memcpy((uintptr_t)to_addr, (uintptr_t)from_addr, num);
        to_addr += To->Pitch;
        from_addr += From->Pitch;
    }
}

uint64_t GetEventData(graphiceventbuffer_t* EventBuffer, uint64_t PositionX, uint64_t PositionY){
    uint64_t Index = PositionX * EventBuffer->Btpp + PositionY * EventBuffer->Pitch;
    return *(uint64_t*)((uint64_t)EventBuffer->Buffer + Index);
}