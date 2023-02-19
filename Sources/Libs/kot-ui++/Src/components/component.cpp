#include <kot-ui++/component.h>

namespace Ui {

    Component::Component(ComponentStyle Style, UpdateHandler HandlerUpdate, MouseEventHandler HandlerMouseEvent, uintptr_t ExternalData, UiContext* ParentUiContex, bool IsUpdateChild){
        /* Style */
        ComponentStyle* CpntStyle = (ComponentStyle*)malloc(sizeof(ComponentStyle));

        /* Layout */
        memcpy(CpntStyle, &Style, sizeof(ComponentStyle));

        CreateFramebuffer(CpntStyle->Width, CpntStyle->Height);

        /* Component */
        this->Style = CpntStyle;
        this->Childs = NULL;
        this->Parent = NULL;
        this->Deep = 0;
        this->UiCtx = ParentUiContex;
        this->UpdateFunction = HandlerUpdate;
        this->MouseEvent = HandlerMouseEvent;
        this->ExternalData = ExternalData;
        this->AbsolutePosition = {.x = 0, .y = 0};
        this->UpdateChild = IsUpdateChild;
        if(Style.IsHorizontalOverflow){
            this->HorizontalOverflow = this;
        }
        if(Style.IsVerticalOverflow){
            this->VerticalOverflow = this;
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

            this->Style->Width = Width;
            this->Style->Height = Height;

            this->IsFramebufferUpdate = true;

            Framebuffer->Size = Height * Pitch;
            Framebuffer->Buffer = calloc(Framebuffer->Size);
            Framebuffer->Pitch = Pitch;
            Framebuffer->Width = Width;
            Framebuffer->Height = Height;
        }

    }
    
    framebuffer_t* Component::GetFramebuffer() {
        return this->Framebuffer;
    }

    Component::ComponentStyle* Component::GetStyle() {
        return this->Style;
    }

    vector_t* Component::GetChilds() {
        return this->Childs;
    }

    void Component::Update(){
        if(Childs != NULL && UpdateChild && Style->IsVisible){
            for(uint64_t i = 0; i < Childs->length; i++) {
                Component* Child = (Component*)vector_get(Childs, i);
                Child->UpdateFunction(Child);
                Child->Update();
            }
        }
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