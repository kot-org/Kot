#include <window/window.h>

windowc::windowc(uint64_t WindowType){
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

KResult windowc::CreateBuffer() {
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

framebuffer_t* windowc::GetFramebuffer(){
    return this->Framebuffer;
}

ksmem_t windowc::GetFramebufferKey(){
    return this->FramebufferKey;
}

KResult windowc::Resize(int64_t Width, int64_t Height){
    if(Width == Window_Max_Size){
        Width = 0;
        for(uint64_t i = 0; i < Monitors->length; i++){
            monitorc* Monitor = (monitorc*)vector_get(Monitors, i);
            if(Monitor != NULL){
                if(IsBeetween(Monitor->XPosition, this->XPosition, Monitor->XMaxPosition)){
                    Width = Monitor->XMaxPosition - this->XPosition;
                }
            }
        }
    }

    if(Height == Window_Max_Size){
        Height = 0;
        for(uint64_t i = 0; i < Monitors->length; i++){
            monitorc* Monitor = (monitorc*)vector_get(Monitors, i);
            if(Monitor != NULL){
                if(IsBeetween(Monitor->YPosition, this->YPosition, Monitor->YMaxPosition)){
                    Height = Monitor->YMaxPosition - this->YPosition;
                }
            }
        }
    }

    this->Framebuffer->Width = Width;
    this->Framebuffer->Height = Height;
    CreateBuffer();
    return KSUCCESS;
}

KResult windowc::Move(int64_t XPosition, int64_t YPosition){
    this->XPosition = XPosition;
    this->YPosition = YPosition;
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


bool windowc::SetState(bool IsFocus){
    this->IsFocus = IsFocus;
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

bool windowc::GetVisible() {
    return IsVisible;
}

KResult windowc::Close() {
    SetVisible(false);
    free(this);

    return KSUCCESS;
}