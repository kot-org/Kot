#include <window/window.h>

windowc* CurrentFocusWindow = NULL;

windowc* WindowBackgroundStart = NULL;
windowc* WindowDefaultStart = NULL;
windowc* WindowForegroundStart = NULL;
windowc* WindowBackgroundEnd = NULL;
windowc* WindowDefaultEnd = NULL;
windowc* WindowForegroundEnd = NULL;

windowc::windowc(orbc* Parent, uint64_t WindowType, kot_event_t Event){
    Orb = Parent;

    this->Framebuffer = (kot_framebuffer_t*)calloc(sizeof(kot_framebuffer_t));

    this->Framebuffer->Bpp = DEFAUT_BPP;
    this->Framebuffer->Btpp = DEFAUT_BPP / 8;
    
    this->Lock = 0;
    
    this->Eventbuffer = CreateEventBuffer(NULL, NULL);

    this->WindowType = WindowType;

    this->Event = Event;
    
    this->MouseEvent = (hid_event_t*)malloc(sizeof(hid_event_t));
    this->MouseEvent->Event = Event;
    this->MouseEvent->ParentType = MOUSE_EVENT_PARENT_TYPE_WINDOW;
    this->MouseEvent->Parent = this;

    this->XPosition = NULL;
    this->YPosition = NULL;

    this->Next = NULL;
    this->Last = NULL;
    
    this->SetFocusState(false);
    this->SetVisible(false);

    CreateBuffer();
}

KResult windowc::CreateBuffer(){
    void* OldFramebuffer = this->Framebuffer->Buffer;
    kot_key_mem_t OldFramebufferKey = this->FramebufferKey;

    this->Framebuffer->Pitch = this->Framebuffer->Width * this->Framebuffer->Btpp;
    this->Framebuffer->Size = this->Framebuffer->Pitch * this->Framebuffer->Height;

    void* Address = GetFreeAlignedSpace(this->Framebuffer->Size);
    kot_key_mem_t Key = NULL;
    Sys_CreateMemoryField(Sys_GetProcess(), this->Framebuffer->Size, &Address, &Key, MemoryFieldTypeShareSpaceRW);
    kot_key_mem_t KeyShare = NULL;
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

    void* OldEventBuffer = Eventbuffer->Buffer;
    Eventbuffer->Buffer = malloc(Eventbuffer->Size);
    memset64(Eventbuffer->Buffer, (uint64_t)this->MouseEvent, Eventbuffer->Size);
    free(OldEventBuffer);

    return KSUCCESS;
}

monitorc* windowc::FindMonitor(){
    for(uint64_t i = 0; i < Orb->Render->Monitors->length; i++){
        monitorc* Monitor = (monitorc*)vector_get(Orb->Render->Monitors, i);
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

kot_framebuffer_t* windowc::GetFramebuffer(){
    return this->Framebuffer;
}

kot_key_mem_t windowc::GetFramebufferKey(){
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

    atomicAcquire(&Lock, 0);
    Framebuffer->Width = Width;
    Framebuffer->Height = Height;
    Eventbuffer->Width = Width;
    Eventbuffer->Height = Height;

    CreateBuffer();
    atomicUnlock(&Lock, 0);

    if(GetVisible()){
        Orb->Render->UpdateAllEvents();
    }

    return KSUCCESS;
}

KResult windowc::Move(int64_t XPosition, int64_t YPosition){
    if(XPosition < 0){
        XPosition = 0;
    }
    if(YPosition < 0){
        YPosition = 0;
    }
    this->XPosition = XPosition;
    this->YPosition = YPosition;
    if(GetVisible()){
        Orb->Render->UpdateAllEvents();
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
    if(this->IsFocus != IsFocus){
        this->IsFocus = IsFocus;
        if(this->IsFocus){
            if(CurrentFocusWindow != NULL) CurrentFocusWindow->SetFocusState(false);
            CurrentFocusWindow = this;
            if(WindowType == Window_Type_Default && IsVisible){
                atomicAcquire(&Orb->Render->RenderMutex, 0);
                
                this->DequeuWL();
                this->EnqueuWL();

                atomicUnlock(&Orb->Render->RenderMutex, 0);

                Orb->Render->UpdateAllEvents();
            }
        }

        arguments_t Parameters{
            .arg[0] = Window_Event_Focus,   // Event type
            .arg[1] = IsFocus,              // Focus state
        };
        Sys_Event_Trigger(Event, &Parameters);
    }

    return this->IsFocus;
}

bool windowc::GetFocusState(){
    return IsFocus;
}


bool windowc::SetVisible(bool IsVisible){
    if(this->IsVisible != IsVisible){
        if(IsVisible){
            this->Enqueu();
            if(CurrentFocusWindow == NULL){
                CurrentFocusWindow = this;
            }
        }else{
            this->Dequeu();
        }
        this->IsVisible = IsVisible;
        Orb->Render->UpdateAllEvents();
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

KResult windowc::Enqueu(){
    atomicAcquire(&Orb->Render->RenderMutex, 0);
    KResult Status = EnqueuWL();
    atomicUnlock(&Orb->Render->RenderMutex, 0);
    return Status;
}

KResult windowc::EnqueuWL(){
    if(this->WindowType == Window_Type_Default){
        if(WindowDefaultStart == NULL){
            WindowDefaultStart = this;

            if(WindowBackgroundEnd){
                WindowBackgroundEnd->Next = this;
                this->Last = WindowBackgroundEnd;
            }else{
                Orb->Render->FirstWindowNode = WindowDefaultStart;
                if(WindowForegroundStart){
                    WindowDefaultStart->Next = WindowForegroundStart;
                    WindowForegroundStart->Last = WindowDefaultStart;                    
                }
            }
        }else{
            this->Last = WindowDefaultEnd;
            WindowDefaultEnd->Next = this;
        }
        this->Next = WindowForegroundStart;
        if(WindowForegroundStart){
            WindowForegroundStart->Last = this;
        }
        WindowDefaultEnd = this;
    }else if(this->WindowType == Window_Type_Foreground){
        if(WindowForegroundStart == NULL){
            WindowForegroundStart = this;
            if(WindowDefaultEnd){
                WindowDefaultEnd->Next = WindowForegroundStart;
                WindowForegroundStart->Last = WindowDefaultEnd;
            }else if(WindowBackgroundEnd){
                WindowBackgroundEnd->Next = WindowForegroundStart;
                WindowForegroundStart->Last = WindowBackgroundEnd;
            }else{
                Orb->Render->FirstWindowNode = WindowForegroundStart;
            }
        }else{
            this->Next = WindowForegroundStart->Next;
            WindowForegroundStart->Next = this;
            this->Last = WindowForegroundStart;
        }
        WindowForegroundEnd = this;
    }
    return KSUCCESS;
}

KResult windowc::Dequeu(){
    atomicAcquire(&Orb->Render->RenderMutex, 0);
    KResult Status = DequeuWL();
    atomicUnlock(&Orb->Render->RenderMutex, 0);
    return Status;
}

KResult windowc::DequeuWL(){
    if(this->Last){
        this->Last->Next = this->Next;
    }

    if(this->Next){
        this->Next->Last = this->Last;
    }

    if(this == WindowBackgroundStart){
        WindowBackgroundStart = NULL;
    }

    if(this == WindowBackgroundEnd){
        WindowBackgroundEnd = NULL;
    }

    if(this == WindowDefaultStart){
        if(this->Next){
            if(this->Next->WindowType == Window_Type_Default){
                WindowDefaultStart = this->Next;
            }else{
                WindowDefaultStart = NULL;
            }
        }else{
            WindowDefaultStart = NULL;
        }
    }

    if(this == WindowDefaultEnd){
        if(this->Last){
            if(this->Last->WindowType == Window_Type_Default){
                WindowDefaultEnd = this->Last;
            }else{
                WindowDefaultEnd = NULL;
            }
        }else{
            WindowDefaultEnd = NULL;
        }
    }


    if(this == WindowForegroundStart){
        if(this->Next){
            if(this->Next->WindowType == Window_Type_Foreground){
                WindowForegroundStart = this->Next;
            }else{
                WindowForegroundStart = NULL;
            }
        }else{
            WindowForegroundStart = NULL;
        }
    }

    if(this == WindowForegroundEnd){
        if(this->Last){
            if(this->Last->WindowType == Window_Type_Foreground){
                WindowForegroundEnd = this->Last;
            }else{
                WindowForegroundEnd = NULL;
            }
        }else{
            WindowForegroundEnd = NULL;
        }
    }

    return KSUCCESS;
}