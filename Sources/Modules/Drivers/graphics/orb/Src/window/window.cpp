#include <window/window.h>

window_c::window_c(uint64_t WindowType){
    this->Framebuffer = (framebuffer_t*)calloc(sizeof(framebuffer_t));

    this->Framebuffer->Bpp = DEFAUT_Bpp;
    this->Framebuffer->Btpp = DEFAUT_Bpp / 8;

    this->WindowType = Window_Type_Default;

    if(WindowType == Window_Type_Background){
        this->WindowType = WindowType;
    }else if(WindowType == Window_Type_Foreground){
        this->WindowType = WindowType;
    }

    this->XPosition = NULL;
    this->YPosition = NULL;
    
    this->SetState(false);
    this->SetVisible(false);

    CreateBuffer();
}

KResult window_c::CreateBuffer() {
    if (this->Framebuffer->Buffer != NULL && this->FramebufferKey != NULL) {
        Sys_CloseMemoryField(Sys_GetProcess(), this->FramebufferKey, this->Framebuffer->Buffer);
    }

    this->Framebuffer->Pitch = this->Framebuffer->Width * this->Framebuffer->Btpp;
    this->Framebuffer->Size = Framebuffer->Pitch * this->Framebuffer->Height;

    uintptr_t address = GetFreeAlignedSpace(this->Framebuffer->Size);
    ksmem_t key = NULL;
    Sys_CreateMemoryField(Sys_GetProcess(), this->Framebuffer->Size, &address, &key, MemoryFieldTypeShareSpaceRW);
    ksmem_t KeyShare = NULL;
    Sys_Keyhole_CloneModify(key, &KeyShare, NULL, KeyholeFlagPresent, PriviledgeApp);

    this->Framebuffer->Buffer = address;
    this->FramebufferKey = KeyShare;

    // clear window buffer
    memset(address, NULL, this->Framebuffer->Size);

    return KSUCCESS;
}

framebuffer_t* window_c::GetFramebuffer(){
    return this->Framebuffer;
}

ksmem_t window_c::GetFramebufferKey(){
    return this->FramebufferKey;
}

KResult window_c::Resize(int64_t Width, int64_t Height){
    if(Width == Window_Max_Size){
        for(uint64_t i = 0; i < Monitors->length; i++){
            monitor_c* Monitor = (monitor_c*)vector_get(Monitors, i);
            if(Monitor != NULL){
                if(IsBeetween(Monitor->XPosition, this->XPosition)
            }
        }
    }

    if(Height == Window_Max_Size){

    }

    this->Framebuffer->Width = Width;
    this->Framebuffer->Height = Height;
    CreateBuffer();
    return KSUCCESS;
}

KResult window_c::Move(int64_t XPosition, int64_t YPosition){
    this->XPosition = XPosition;
    this->YPosition = YPosition;
    return KSUCCESS;
}

uint64_t window_c::GetHeight(){
    return Framebuffer->Height;
}

uint64_t window_c::GetWidth(){
    return Framebuffer->Width;
}

uint64_t window_c::GetBpp(){
    return Framebuffer->Bpp;
}

uint64_t window_c::GetX(){
    return XPosition;
}

uint64_t window_c::GetY(){
    return YPosition;
}


bool window_c::SetState(bool IsFocus){
    this->IsFocus = IsFocus;
    return this->IsFocus;
}

bool window_c::GetState(){
    return IsFocus;
}


bool window_c::SetVisible(bool IsVisible){
    if(this->IsVisible != IsVisible){
        if(this->WindowType == Window_Type_Background){
            if(IsVisible){
                this->WindowIndex = vector_push(Background, this);
            }else{
                vector_remove(Background, this->WindowIndex);
            }
        }else if(this->WindowType == Window_Type_Default){
            if(IsVisible){
                this->WindowIndex = vector_push(Windows, this);
            }else{
                vector_remove(Windows, this->WindowIndex);
            }
        }else if(this->WindowType == Window_Type_Foreground){
            if(IsVisible){
                this->WindowIndex = vector_push(Foreground, this);
            }else{
                vector_remove(Foreground, this->WindowIndex);
            }
        }
        this->IsVisible = IsVisible;
    }
    return this->IsVisible;
}

bool window_c::GetVisible() {
    return IsVisible;
}

KResult window_c::Close() {
    SetVisible(false);
    free(this);

    return KSUCCESS;
}