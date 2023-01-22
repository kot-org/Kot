#include <window/window.h>

windowc* CurrentFocusWindow = NULL;

windowc::windowc(uint64_t WindowType, event_t Event){
    this->Framebuffer = (framebuffer_t*)calloc(sizeof(framebuffer_t));

    this->Framebuffer->Bpp = DEFAUT_BPP;
    this->Framebuffer->Btpp = DEFAUT_BPP / 8;
    
    this->Eventbuffer = CreateEventBuffer(NULL, NULL);

    this->Eventbuffer->Bpp = sizeof(windowc*) * 8;
    this->Eventbuffer->Btpp = sizeof(windowc*);

    this->WindowType = WindowType;
    this->Event = Event;

    this->XPosition = NULL;
    this->YPosition = NULL;
    
    this->SetFocusState(false);
    this->SetVisible(false);

    CreateBuffer();
}

KResult windowc::CreateBuffer(){
    uintptr_t OldFramebuffer = this->Framebuffer->Buffer;
    ksmem_t OldFramebufferKey = this->FramebufferKey;

    this->Framebuffer->Pitch = this->Framebuffer->Width * this->Framebuffer->Btpp;
    this->Framebuffer->Size = this->Framebuffer->Pitch * this->Framebuffer->Height;

    uintptr_t Address = GetFreeAlignedSpace(this->Framebuffer->Size);
    ksmem_t Key = NULL;
    Sys_CreateMemoryField(Sys_GetProcess(), this->Framebuffer->Size, &Address, &Key, MemoryFieldTypeShareSpaceRW);
    ksmem_t KeyShare = NULL;
    Sys_Keyhole_CloneModify(Key, &KeyShare, NULL, KeyholeFlagPresent, PriviledgeApp);
    
    Framebuffer->Buffer = Address;
    FramebufferKey = KeyShare;

    if(OldFramebuffer != NULL && OldFramebufferKey != NULL){
        Sys_CloseMemoryField(Sys_GetProcess(), OldFramebufferKey, OldFramebuffer);
    }
    // clear window buffer
    memset(Framebuffer->Buffer, NULL, Framebuffer->Size);


    Eventbuffer->Pitch = Eventbuffer->Width * Eventbuffer->Btpp;
    Eventbuffer->Size = Eventbuffer->Pitch * Eventbuffer->Height;

    uintptr_t OldEventBuffer = Eventbuffer->Buffer;
    Eventbuffer->Buffer = malloc(Eventbuffer->Size);
    memset64(Eventbuffer->Buffer, (uint64_t)this, Eventbuffer->Size);
    free(OldEventBuffer);

    return KSUCCESS;
}

monitorc* windowc::FindMonitor(){
    for(uint64_t i = 0; i < Monitors->length; i++){
        monitorc* Monitor = (monitorc*)vector_get(Monitors, i);
        if(Monitor != NULL){
            if(IsBeetween(Monitor->YPosition, this->YPosition, Monitor->YMaxPosition)){
                return Monitor;
            }
        }
    }
    return NULL;
}

graphiceventbuffer_t* windowc::GetEventbuffer(){
    return this->Eventbuffer;
}

framebuffer_t* windowc::GetFramebuffer(){
    return this->Framebuffer;
}

ksmem_t windowc::GetFramebufferKey(){
    return this->FramebufferKey;
}

KResult windowc::Resize(int64_t Width, int64_t Height){
    if(Width == Window_Max_Size){
        monitorc* Monitor = FindMonitor();
        if(Monitor == NULL){
            Width = 0;
        }else{
            Width = Monitor->XMaxPositionWithDock - this->XPosition;
        }
    }

    if(Height == Window_Max_Size){
        monitorc* Monitor = FindMonitor();
        if(Monitor == NULL){
            Height = 0;
        }else{
            Height = Monitor->YMaxPositionWithDock - this->YPosition;
        }
    }

    switch (WindowType){
        case Window_Type_DockTop:{
            monitorc* Monitor = FindMonitor();
            if(Monitor != NULL){
                if(Monitor->DockTop == NULL){
                    Width = Monitor->XMaxPosition - Monitor->XPosition;
                    Monitor->YPositionWithDock = Height;
                    XPosition = Monitor->XPosition;
                    YPosition = Monitor->YPosition;
                    Monitor->DockTop = this;
                }else if(Monitor->DockTop == this){
                    Width = Monitor->XMaxPosition - Monitor->XPosition;
                    Monitor->YMaxPositionWithDock = Height;
                    XPosition = Monitor->XPosition;
                    YPosition = Monitor->YPosition;
                }
            }
            break;
        }
        case Window_Type_DockBottom:{
            monitorc* Monitor = FindMonitor();
            if(Monitor != NULL){
                if(Monitor->DockTop == NULL){
                    Width = Monitor->XMaxPosition - Monitor->XPosition;
                    Monitor->YMaxPositionWithDock = Monitor->YMaxPosition - Height;
                    XPosition = Monitor->XPosition;
                    YPosition = Monitor->YMaxPositionWithDock;
                    Monitor->DockTop = this;
                }else if(Monitor->DockTop == this){
                    Width = Monitor->XMaxPosition - Monitor->XPosition;
                    Monitor->YMaxPositionWithDock = Monitor->YMaxPosition - Height;
                    XPosition = Monitor->XPosition;
                    YPosition = Monitor->YMaxPositionWithDock;
                }
            }
            break;
        }
        case Window_Type_DockLeft:{
            monitorc* Monitor = FindMonitor();
            if(Monitor != NULL){
                if(Monitor->DockTop == NULL){
                    Height = Monitor->YMaxPosition - Monitor->YPosition;
                    Monitor->XPositionWithDock = Width;
                    XPosition = Monitor->XPosition;
                    YPosition = Monitor->YPosition;
                    Monitor->DockTop = this;
                }else if(Monitor->DockTop == this){
                    Height = Monitor->YMaxPosition - Monitor->YPosition;
                    Monitor->XPositionWithDock = Width;
                    XPosition = Monitor->XPosition;
                    YPosition = Monitor->YPosition;
                }
            }
            break;
        }
        case Window_Type_DockRight:{
            monitorc* Monitor = FindMonitor();
            if(Monitor != NULL){
                if(Monitor->DockTop == NULL){
                    Height = Monitor->YMaxPosition - Monitor->YPosition;
                    Monitor->XMaxPositionWithDock = Monitor->XMaxPosition - Width;
                    XPosition = Monitor->XMaxPositionWithDock;
                    YPosition = Monitor->XPosition;
                    Monitor->DockTop = this;
                }else if(Monitor->DockTop == this){
                    Height = Monitor->YMaxPosition - Monitor->YPosition;
                    Monitor->XMaxPositionWithDock = Monitor->XMaxPosition - Width;
                    XPosition = Monitor->XMaxPositionWithDock;
                    YPosition = Monitor->XPosition;
                }
            }
            break;
        }
        default:
            break;
    }

    Framebuffer->Width = Width;
    Framebuffer->Height = Height;
    Eventbuffer->Width = Width;
    Eventbuffer->Height = Height;

    CreateBuffer();

    if(GetVisible()){
        UpdateAllEvents();
    }

    return KSUCCESS;
}

KResult windowc::Move(int64_t XPosition, int64_t YPosition){
    this->XPosition = XPosition;
    this->YPosition = YPosition;
    if(GetVisible()){
        UpdateAllEvents();
    }
    return KSUCCESS;
}

uint64_t windowc::GetHeight(){
    return Framebuffer->Height;
}

uint64_t windowc::GetWidth(){
    return Framebuffer->Width;
}

uint64_t windowc::GetBpp(){
    return Framebuffer->Bpp;
}

uint64_t windowc::GetX(){
    return XPosition;
}

uint64_t windowc::GetY(){
    return YPosition;
}


bool windowc::SetFocusState(bool IsFocus){
    this->IsFocus = IsFocus;

    if(this->IsFocus){
        if(CurrentFocusWindow != NULL) CurrentFocusWindow->SetFocusState(false);
        CurrentFocusWindow = this;
        if(this->WindowType == Window_Type_Default){
            uint64_t NewWindowIndex = vector_push(Windows, this);
            vector_remove(Windows, this->WindowIndex);
            this->WindowIndex = NewWindowIndex;
            UpdateAllEvents();
        }
    }

    arguments_t Parameters{
        .arg[0] = Winwow_Event_Focus,   // Event type
        .arg[1] = IsFocus,              // Focus state
    };
    
    Sys_Event_Trigger(Event, &Parameters);

    return this->IsFocus;
}

bool windowc::GetState(){
    return IsFocus;
}


bool windowc::SetVisible(bool IsVisible){
    if(this->IsVisible != IsVisible){
        if(this->WindowType == Window_Type_Background){
            if(IsVisible){
                this->WindowIndex = vector_push(Background, this);
                if(CurrentFocusWindow == NULL){
                    CurrentFocusWindow = this;
                }
            }else{
                vector_remove(Background, this->WindowIndex);
            }
        }else if(this->WindowType == Window_Type_Default){
            if(IsVisible){
                this->WindowIndex = vector_push(Windows, this);
                if(CurrentFocusWindow == NULL){
                    CurrentFocusWindow = this;
                }
            }else{
                vector_remove(Windows, this->WindowIndex);
            }
        }else if(this->WindowType == Window_Type_Foreground || this->WindowType == Window_Type_DockTop || this->WindowType == Window_Type_DockBottom || this->WindowType == Window_Type_DockLeft || this->WindowType == Window_Type_DockRight){
            if(IsVisible){
                this->WindowIndex = vector_push(Foreground, this);
                if(CurrentFocusWindow == NULL){
                    CurrentFocusWindow = this;
                }
            }else{
                vector_remove(Foreground, this->WindowIndex);
            }
        }
        this->IsVisible = IsVisible;
        UpdateAllEvents();
    }
    

    return this->IsVisible;
}

bool windowc::GetVisible() {
    return IsVisible;
}

KResult windowc::Close() {
    SetVisible(false);
    free(this);

    return KSUCCESS;
}