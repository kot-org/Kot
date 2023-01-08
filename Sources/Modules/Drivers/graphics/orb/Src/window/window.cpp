#include <window/window.h>

Window::Window(uint64_t WindowType){
    this->Framebuffer = (framebuffer_t*)calloc(sizeof(framebuffer_t));

    this->Framebuffer->Bpp = DEFAUT_Bpp;
    this->Framebuffer->Btpp = DEFAUT_Bpp / 8;

    this->WindowType = Window_Type_Default;

    if(WindowType == Window_Type_Background){
        this->WindowType = WindowType;
    }else if(WindowType == Window_Type_Foreground){
        this->WindowType = WindowType;
    }

    this->XPositionition = NULL;
    this->YPositionition = NULL;
    
    this->SetState(false);
    this->SetVisible(false);

    CreateBuffer();
}

KResult Window::CreateBuffer() {
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

framebuffer_t* Window::GetFramebuffer(){
    return this->Framebuffer;
}

ksmem_t Window::GetFramebufferKey(){
    return this->FramebufferKey;
}

KResult Window::Resize(int64_t Width, int64_t Height){
    this->Framebuffer->Width = Width;
    this->Framebuffer->Height = Height;
    CreateBuffer();
    return KSUCCESS;
}

KResult Window::Move(int64_t XPositionition, int64_t YPositionition){
    this->XPositionition = XPositionition;
    this->YPositionition = YPositionition;
    return KSUCCESS;
}

uint64_t Window::GetHeight(){
    return Framebuffer->Height;
}

uint64_t Window::GetWidth(){
    return Framebuffer->Width;
}

uint64_t Window::GetBpp(){
    return Framebuffer->Bpp;
}

uint64_t Window::GetX(){
    return XPositionition;
}

uint64_t Window::GetY(){
    return YPositionition;
}


void Window::SetState(bool IsFocus){
    this->IsFocus = IsFocus;
}

bool Window::GetState(){
    return IsFocus;
}


void Window::SetVisible(bool IsVisible){
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
}

bool Window::GetVisible() {
    return IsVisible;
}

void Window::Close() {
    SetVisible(false);
    free(this);
}