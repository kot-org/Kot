#include <kot-ui++/component.h>

namespace Ui {

    Component::Component(ComponentGeneralStyle Style, UpdateHandler HandlerUpdate, MouseEventHandler HandlerMouseEvent, uintptr_t ExternalData, Component* ParentCpnt, bool IsUpdateChild, bool IsOwnFb){
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

        if(CpntStyle->Width < 0){
            CpntStyle->Currentwidth = (Parent->Style->Currentwidth * abs(CpntStyle->Width)) / 100;
            if(CpntStyle->Currentwidth < CpntStyle->Minwidth){
                CpntStyle->Currentwidth = CpntStyle->Minwidth;
            }else if(CpntStyle->Currentwidth < CpntStyle->Maxwidth){
                CpntStyle->Currentwidth = CpntStyle->Maxwidth;
            }
        }else{
            CpntStyle->Currentwidth = CpntStyle->Width;
        }

        if(CpntStyle->Height < 0){
            CpntStyle->Currentheight = (Parent->Style->Currentheight * abs(CpntStyle->Height)) / 100;
            if(CpntStyle->Currentheight < CpntStyle->Minheight){
                CpntStyle->Currentheight = CpntStyle->Minheight;
            }else if(CpntStyle->Currentheight < CpntStyle->Maxheight){
                CpntStyle->Currentheight = CpntStyle->Maxheight;
            }
        }else{
            CpntStyle->Currentheight = CpntStyle->Height;
        }

        if(IsOwnFb){
            CreateFramebuffer(CpntStyle->Currentwidth, CpntStyle->Currentheight);
        }else{
            this->Framebuffer = this->Parent->Framebuffer;
        }

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

            this->Style->Currentwidth = Width;
            this->Style->Currentheight = Height;

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
            uint64_t XIteration = 0;
            uint64_t YIteration = 0;
            uint64_t MaxChildHeight = 0;
            for(uint64_t i = 0; i < Childs->length; i++) {
                Component* Child = (Component*)vector_get(Childs, i);

                // Calculate width and height
                if(Child->Style->Width < 0){
                    Child->Style->Currentwidth = (Style->Currentwidth * abs(Child->Style->Width)) / 100;
                    if(Child->Style->Currentwidth < Child->Style->Minwidth){
                        Child->Style->Currentwidth = Child->Style->Minwidth;
                    }else if(Child->Style->Currentwidth < Child->Style->Maxwidth){
                        Child->Style->Currentwidth = Child->Style->Maxwidth;
                    }
                }else{
                    Child->Style->Currentwidth = Child->Style->Width;
                }

                if(Child->Style->Height < 0){
                    Child->Style->Currentheight = (Style->Currentheight * abs(Child->Style->Height)) / 100;
                    if(Child->Style->Currentheight < Child->Style->Minheight){
                        Child->Style->Currentheight = Child->Style->Minheight;
                    }else if(Child->Style->Currentheight < Child->Style->Maxheight){
                        Child->Style->Currentheight = Child->Style->Maxheight;
                    }
                }else{
                    Child->Style->Currentheight = Child->Style->Height;
                }

                if(Child->Style->AutoPosition){
                    Child->Style->Position.x = XIteration;
                    Child->Style->Position.y = YIteration;

                    XIteration += Child->Style->Width + Child->Style->Margin.Left + Child->Style->Margin.Right;

                    if(XIteration > Parent->Style->Width){
                        YIteration += MaxChildHeight;
                    }

                    if(MaxChildHeight < Child->Style->Height + Child->Style->Margin.Top + Child->Style->Margin.Bottom){
                        MaxChildHeight = Child->Style->Height + Child->Style->Margin.Top + Child->Style->Margin.Bottom;
                    }
                }

                Child->UpdateFunction(Child);
                Child->Update();
            }
        }
        IsRedraw = false;
    }

    void Component::AddChild(Component* Child) {
        // TODO sort by position first and by z-index
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