#include <kot-ui++/component.h>

namespace Ui {

    Component::Component(ComponentGeneralStyle Style, UpdateHandler HandlerUpdate, MouseEventHandler HandlerMouseEvent, uintptr_t ExternalData, Component* ParentCpnt, bool IsUpdateChild){
        /* Style */
        ComponentGeneralStyle* CpntStyle = (ComponentGeneralStyle*)malloc(sizeof(ComponentGeneralStyle));

        /* Component */
        this->Style = CpntStyle;
        this->Childs = NULL;
        this->Deep = 0;
        this->Parent = ParentCpnt;
        this->UpdateFunction = HandlerUpdate;
        this->MouseEvent = HandlerMouseEvent;
        this->ExternalData = ExternalData;
        this->AbsolutePosition = {.x = 0, .y = 0};
        this->UpdateChild = IsUpdateChild;
        this->IsRedraw = true;

        /* Layout */
        memcpy(CpntStyle, &Style, sizeof(ComponentGeneralStyle));

        if(CpntStyle->Width.Normal < 0){
            CpntStyle->Width.Current = (Parent->Style->Width.Current * abs(CpntStyle->Width.Normal)) / 100;
            if(CpntStyle->Width.Current < CpntStyle->Width.Min){
                CpntStyle->Width.Current = CpntStyle->Width.Min;
            }else if(CpntStyle->Width.Current < CpntStyle->Width.Max){
                CpntStyle->Width.Current = CpntStyle->Width.Max;
            }
        }else{
            CpntStyle->Width.Current = CpntStyle->Width.Normal;
        }

        if(CpntStyle->Height.Normal < 0){
            CpntStyle->Height.Current = (Parent->Style->Height.Current * abs(CpntStyle->Height.Normal)) / 100;
            if(CpntStyle->Height.Current < CpntStyle->Height.Min){
                CpntStyle->Height.Current = CpntStyle->Height.Min;
            }else if(CpntStyle->Height.Current < CpntStyle->Height.Max){
                CpntStyle->Height.Current = CpntStyle->Height.Max;
            }
        }else{
            CpntStyle->Height.Current = CpntStyle->Height.Normal;
        }

        CreateFramebuffer(CpntStyle->Width.Current, CpntStyle->Height.Current);

        if(Style.IsHorizontalOverflow){
            this->HorizontalOverflow = this;
        }
        if(Style.IsVerticalOverflow){
            this->VerticalOverflow = this;
        }

        if(this->Parent){
            this->UiCtx = Parent->UiCtx;
            ParentCpnt->AddChild(this);
        }
    }

    /* Component Framebuffer */
    void Component::CreateFramebuffer(uint32_t Width, uint32_t Height) {
        framebuffer_t* CpntFb = (framebuffer_t*) malloc(sizeof(framebuffer_t));

        uint32_t Bpp = 32, Btpp = 4;

        uint32_t Pitch = Width * Btpp;

        CpntFb->Size = Height * Pitch;
        CpntFb->Buffer = calloc(CpntFb->Size);
        CpntFb->Pitch = Pitch;
        CpntFb->Width = Width;
        CpntFb->Height = Height;
        CpntFb->Bpp = Bpp;
        CpntFb->Btpp = Btpp;

        this->Framebuffer = CpntFb;
        this->IsFramebufferUpdate = true;
    }
    
    void Component::UpdateFramebuffer(uint32_t Width, uint32_t Height) {
        if(Framebuffer->Width != Width || Framebuffer->Height != Height) {
            uint32_t Pitch = Width * sizeof(uint32_t);

            this->Style->Width.Current = Width;
            this->Style->Height.Current = Height;

            this->IsFramebufferUpdate = true;

            Framebuffer->Size = Height * Pitch;
            uintptr_t OldBuffer = Framebuffer->Buffer;
            Framebuffer->Buffer = calloc(Framebuffer->Size);
            free(OldBuffer);
            Framebuffer->Pitch = Pitch;
            Framebuffer->Width = Width;
            Framebuffer->Height = Height;
            this->IsFramebufferUpdate = true;
        }
    }
    
    void Component::UpdateFramebuffer(framebuffer_t* fb) {
        uintptr_t OldBuffer = Framebuffer->Buffer;
        memcpy(Framebuffer, fb, sizeof(framebuffer_t));
        this->IsFramebufferUpdate = true;
        free(OldBuffer);
    }
    
    framebuffer_t* Component::GetFramebuffer() {
        return this->Framebuffer;
    }

    ComponentGeneralStyle* Component::GetStyle() {
        return this->Style;
    }

    vector_t* Component::GetChilds() {
        return this->Childs;
    }

    void Component::Update(){
        if(Childs != NULL && UpdateChild && Style->IsVisible){
            for(uint64_t i = 0; i < Childs->length; i++) {
                Component* Child = (Component*)vector_get(Childs, i);
                if(Child->Style->Width.Normal < 0){
                    Child->Style->Width.Current = (Style->Width.Current * abs(Child->Style->Width.Normal)) / 100;
                    if(Child->Style->Width.Current < Child->Style->Width.Min){
                        Child->Style->Width.Current = Child->Style->Width.Min;
                    }else if(Child->Style->Width.Current < Child->Style->Width.Max){
                        Child->Style->Width.Current = Child->Style->Width.Max;
                    }
                }else{
                    Child->Style->Width.Current = Child->Style->Width.Normal;
                }

                if(Child->Style->Height.Normal < 0){
                    Child->Style->Height.Current = (Style->Height.Current * abs(Child->Style->Height.Normal)) / 100;
                    if(Child->Style->Height.Current < Child->Style->Height.Min){
                        Child->Style->Height.Current = Child->Style->Height.Min;
                    }else if(Child->Style->Height.Current < Child->Style->Height.Max){
                        Child->Style->Height.Current = Child->Style->Height.Max;
                    }
                }else{
                    Child->Style->Height.Current = Child->Style->Height.Normal;
                }
                Child->UpdateFunction(Child);
                Child->Update();
            }
        }
        IsRedraw = false;
    }

    void Component::AddChild(Component* Child) {
        Child->Deep = this->Deep + 1;

        if(!this->Childs){
            this->Childs = vector_create();
        }

        Child->HorizontalOverflow = HorizontalOverflow;
        Child->VerticalOverflow = VerticalOverflow;
        
        Child->Parent = this;
        vector_push(this->Childs, Child);
    } 
}