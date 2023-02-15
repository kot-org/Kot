#include <kot-ui++/component.h>

namespace Ui {

    Component::Component(ComponentStyle Style, UpdateHandler HandlerUpdate, MouseEventHandler HandlerMouseEvent, uintptr_t ExternalData, UiContext* ParentUiContex, bool IsUpdateChild){
        /* Style */
        ComponentStyle* CpntStyle = (ComponentStyle*)malloc(sizeof(ComponentStyle));

        // layout
        memcpy(CpntStyle, &Style, sizeof(ComponentStyle));

        CreateFramebuffer(CpntStyle->Width, CpntStyle->Height);

        /* component */
        this->Style = CpntStyle;
        this->Childs = NULL;
        this->Parent = NULL;
        this->Deep = 0;
        this->UiCtx = ParentUiContex;
        this->UpdateFunction = HandlerUpdate;
        this->MouseEvent = HandlerMouseEvent;
        this->ExternalData = ExternalData;
        this->UpdateChild = IsUpdateChild;
        this->AbsolutePosition = {.x = 0, .y = 0};
    }

    Component::Component(framebuffer_t* fb, UpdateHandler HandlerUpdate, MouseEventHandler HandlerMouseEvent, UiContext* ParentUiContex, bool IsUpdateChild) {
        /* framebuffer */
        framebuffer_t* CpntFb = (framebuffer_t*) malloc(sizeof(framebuffer_t));

        CpntFb->Size = fb->Size;
        CpntFb->Buffer = fb->Buffer;
        CpntFb->Pitch = fb->Pitch;
        CpntFb->Width = fb->Width;
        CpntFb->Height = fb->Height;
        CpntFb->Bpp = fb->Bpp;
        CpntFb->Btpp = fb->Btpp;

        /* Style */
        ComponentStyle* cpntStyle = (ComponentStyle*) malloc(sizeof(ComponentStyle));

        cpntStyle->Width = fb->Width;
        cpntStyle->Height = fb->Height;

        /* component */
        this->Childs = NULL;
        this->Parent = NULL;
        this->Deep = 0;
        this->Framebuffer = CpntFb;
        this->Style = cpntStyle;
        this->UiCtx = ParentUiContex;
        this->UpdateFunction = HandlerUpdate;
        this->MouseEvent = HandlerMouseEvent;
        this->UpdateChild = IsUpdateChild;
        this->AbsolutePosition = {.x = 0, .y = 0};
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
            uint32_t Pitch = Width * 4;

            Framebuffer->Size = Height * Pitch;
            Framebuffer->Buffer = calloc(Framebuffer->Size);
            Framebuffer->Pitch = Pitch;
            Framebuffer->Width = Width;
            Framebuffer->Height = Height;

            this->IsFramebufferUpdate = true;
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

    void Component::Update() {
        if(Childs != NULL && UpdateChild){
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
        
        Child->Parent = this;
        vector_push(this->Childs, Child);
    } 
}