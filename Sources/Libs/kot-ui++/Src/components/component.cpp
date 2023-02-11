#include <kot-ui++/component.h>

namespace Ui {

    Component::Component(ComponentStyle Style){
        /* Style */
        ComponentStyle* CpntStyle = (ComponentStyle*) malloc(sizeof(ComponentStyle));

        // layout
        CpntStyle->Position = Style.Position;
        CpntStyle->Display = Style.Display;
        CpntStyle->Direction = Style.Direction;
        CpntStyle->Align = Style.Align;
        CpntStyle->Space = Style.Space;

        CpntStyle->Width = Style.Width;
        CpntStyle->Height = Style.Height;
        CpntStyle->FontSize = Style.FontSize;
        CpntStyle->BorderRadius = Style.BorderRadius;

        CpntStyle->BackgroundColor = Style.BackgroundColor;
        CpntStyle->ForegroundColor = Style.ForegroundColor;

        CpntStyle->X = Style.X; 
        CpntStyle->Y = Style.Y;


        CreateFramebuffer(CpntStyle->Width, CpntStyle->Height);

        /* component */
        this->Style = CpntStyle;
        this->Deep = 0;
    }

    Component::Component(framebuffer_t* fb) {
        /* framebuffer */
        framebuffer_t* cpntFb = (framebuffer_t*) malloc(sizeof(framebuffer_t));

        cpntFb->Size = fb->Size;
        cpntFb->Buffer = fb->Buffer;
        cpntFb->Pitch = fb->Pitch;
        cpntFb->Width = fb->Width;
        cpntFb->Height = fb->Height;
        cpntFb->Bpp = fb->Bpp;
        cpntFb->Btpp = fb->Btpp;

        /* Style */
        ComponentStyle* cpntStyle = (ComponentStyle*) malloc(sizeof(ComponentStyle));

        cpntStyle->Width = fb->Width;
        cpntStyle->Height = fb->Height;

        /* component */
        this->Framebuffer = cpntFb;
        this->Style = cpntStyle;
    }

    /* Component Framebuffer */
    void Component::CreateFramebuffer(uint32_t Width, uint32_t Height) {
        framebuffer_t* cpntFb = (framebuffer_t*) malloc(sizeof(framebuffer_t));

        uint32_t Bpp = 32, Btpp = 4;

        uint32_t Pitch = Width * Btpp;

        cpntFb->Size = Height * Pitch;
        cpntFb->Buffer = calloc(cpntFb->Size);
        cpntFb->Pitch = Pitch;
        cpntFb->Width = Width;
        cpntFb->Height = Height;
        cpntFb->Bpp = Bpp;
        cpntFb->Btpp = Btpp;

        this->Framebuffer = cpntFb;
    }
    
    void Component::UpdateFramebuffer(uint32_t Width, uint32_t Height) {

        if(Framebuffer->Width != Width || Framebuffer->Height != Height) {
            uint32_t Pitch = Width * 4;

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

    uint32_t Component::GetTotalWidthChilds() {
        return this->TotalWidthChilds;
    }

    uint32_t Component::GetTotalHeightChilds() {
        return this->TotalHeightChilds;
    }

/*     void Component::update() {
        UiLayout::CalculateLayout(this);
        
        this->draw();
        
        if(childs != NULL) {

            for(int i = 0; i < childs->length; i++) {
                Component* child = (Component*) vector_get(childs, i);

                if(child->fb == NULL)
                    child->draw();
                
                BlitFramebuffer(this->fb, child->fb, child->Style->X, child->Style->Y);
                child->IsBlit = true;
            }

        }

        if(parent != NULL)
            parent->update();
    } */

    void Component::Update() {
        UiLayout::CalculateLayout(this);

        this->Draw();

        if(Childs != NULL) {

            for(uint64_t i = 0; i < Childs->length; i++) {
                Component* child = (Component*) vector_get(Childs, i);


                if(child->ReadyToBlit == true){
                    BlitFramebuffer(this->Framebuffer, child->Framebuffer, child->Style->X, child->Style->Y);
                    
                }
                else{
                    child->Update();
                }

            }

        }
        this->ReadyToBlit = true;

        if(Parent != NULL)
            Parent->Update();
    }

    void Component::Draw() {
        UpdateFramebuffer(this->Style->Width, this->Style->Height);
        //Printlog("draw");

        FillRect(this->Framebuffer, 0, 0, this->Style->Width, this->Style->Height, this->Style->BackgroundColor);
    }

    void Component::AddChild(Component* Child) {
        Child->UiCtx = this->UiCtx;
        Child->Deep = this->Deep + 1;

        if(!this->Childs){
            this->Childs = vector_create();
        }
        
        Child->Parent = this;
        vector_push(this->Childs, Child);

        // todo : remove that
        this->TotalWidthChilds += Child->Style->Width;
        // todo: if there is a new line, add the Height to totalHeightChilds (this->totalHeightChilds += child->Style->Height;)
    }  


    void Component::MouseEvent(uint64_t RelativePositionX, uint64_t RelativePositionY, uint64_t PositionX, uint64_t PositionY, uint64_t ZValue, uint64_t Status){
        Printlog("ok");
    }

}