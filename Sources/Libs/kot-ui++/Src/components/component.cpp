#include <kot-ui++/component.h>
#include <string.h>

namespace Ui {

    Component::Component(ComponentGeneralStyle Style, UpdateHandler HandlerUpdate, MouseEventHandler HandlerMouseEvent, void* ExternalData, Component* ParentCpnt, bool IsOwnFb){
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
        this->FramebufferRelativePosition = {.x = 0, .y = 0};
        this->IsRedraw = true;
        this->OwnFb = IsOwnFb;

        /* Layout */
        memcpy(CpntStyle, (void*)&Style, sizeof(ComponentGeneralStyle));

        if(CpntStyle->Width < 0) {
            CpntStyle->Currentwidth = (Parent->Style->Currentwidth * abs(CpntStyle->Width)) / 100;

            if(CpntStyle->Currentwidth < CpntStyle->Minwidth) {
                CpntStyle->Currentwidth = CpntStyle->Minwidth;
            } else if(CpntStyle->Currentwidth > CpntStyle->Maxwidth) {
                CpntStyle->Currentwidth = CpntStyle->Maxwidth;
            }
        } else {
            CpntStyle->Currentwidth = CpntStyle->Width;
        }

        if(CpntStyle->Height < 0) {
            CpntStyle->Currentheight = (Parent->Style->Currentheight * abs(CpntStyle->Height)) / 100;

            if(CpntStyle->Currentheight < CpntStyle->Minheight) {
                CpntStyle->Currentheight = CpntStyle->Minheight;
            } else if(CpntStyle->Currentheight > CpntStyle->Maxheight) {
                CpntStyle->Currentheight = CpntStyle->Maxheight;
            }
        } else {
            CpntStyle->Currentheight = CpntStyle->Height;
        }

        if(IsOwnFb)
            CreateFramebuffer(CpntStyle->Currentwidth, CpntStyle->Currentheight);
        else {
            this->Framebuffer = this->Parent->Framebuffer;
            this->IsFramebufferUpdate = true;
        }

        if(Style.IsHorizontalOverflow)
            this->HorizontalOverflow = this;
        if(Style.IsVerticalOverflow)
            this->VerticalOverflow = this;

        if(this->Parent) {
            this->UiCtx = Parent->UiCtx;
            ParentCpnt->AddChild(this);
        }
    }

    /* Component Framebuffer */
    void Component::CreateFramebuffer(uint32_t Width, uint32_t Height) {
        kot_framebuffer_t* CpntFb = (kot_framebuffer_t*) malloc(sizeof(kot_framebuffer_t));

        uint32_t Bpp = 32, Btpp = 4;

        uint32_t Pitch = Width * Btpp;

        CpntFb->Size = Height * Pitch;
        CpntFb->Buffer = calloc(CpntFb->Size, Btpp);
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
            if(OwnFb) {
                uint32_t Pitch = Width * sizeof(uint32_t);

                this->Style->Currentwidth = Width;
                this->Style->Currentheight = Height;

                Framebuffer->Size = Height * Pitch;
                void* OldBuffer = Framebuffer->Buffer;
                Framebuffer->Buffer = calloc(Framebuffer->Size, Framebuffer->Btpp);
                free(OldBuffer);
                Framebuffer->Pitch = Pitch;
                Framebuffer->Width = Width;
                Framebuffer->Height = Height;

                this->IsFramebufferUpdate = true;
            } else {
                this->Style->Currentwidth = Width;
                this->Style->Currentheight = Height;

                this->IsFramebufferUpdate = true;
            }
        }
    }
    
    kot_framebuffer_t* Component::GetFramebuffer() {
        return this->Framebuffer;
    }

    ComponentGeneralStyle* Component::GetStyle() {
        return this->Style;
    }

    kot_vector_t* Component::GetChilds() {
        return this->Childs;
    }

    void Component::Update() {
        if(Childs != NULL){
            // Left
            uint64_t XIterationLeft = 0;
            uint64_t YIterationLeft = 0;
            uint64_t MaxChildHeightLeft = 0;

            // Right
            uint64_t XIterationRight = Style->Currentwidth;
            uint64_t YIterationRight = 0;
            uint64_t MaxChildHeightRight = 0;

            for(uint64_t i = 0; i < Childs->length; i++) {
                Component* Child = (Component*)kot_vector_get(Childs, i);

                if(!Child->Style->IsHidden) {

                    // Calculate width and height
                    if(Child->Style->Width < 0) {
                        Child->Style->Currentwidth = (Style->Currentwidth * abs(Child->Style->Width)) / 100;

                        if(Child->Style->Currentwidth < Child->Style->Minwidth) {
                            Child->Style->Currentwidth = Child->Style->Minwidth;
                        } else if(Child->Style->Currentwidth > Child->Style->Maxwidth) {
                            Child->Style->Currentwidth = Child->Style->Maxwidth;
                        }

                        if(Child->Style->Currentwidth + Child->Style->Position.x > Style->Currentwidth) {
                            Child->Style->Currentwidth = Style->Currentwidth - Child->Style->Position.x;
                        }
                    } else {
                        Child->Style->Currentwidth = Child->Style->Width;
                    }

                    if(Child->Style->Height < 0) {
                        Child->Style->Currentheight = (Style->Currentheight * abs(Child->Style->Height)) / 100;

                        if(Child->Style->Currentheight < Child->Style->Minheight) {
                            Child->Style->Currentheight = Child->Style->Minheight;
                        } else if(Child->Style->Currentheight > Child->Style->Maxheight) {
                            Child->Style->Currentheight = Child->Style->Maxheight;
                        }

                        if(Child->Style->Currentheight + Child->Style->Position.y > Style->Currentheight) {
                            Child->Style->Currentheight = Style->Currentheight - Child->Style->Position.y;
                        }
                    } else {
                        Child->Style->Currentheight = Child->Style->Height;
                    }

                    Child->UpdateFramebuffer(Child->Style->Currentwidth, Child->Style->Currentheight);

                    if(Child->Style->AutoPosition) {
                        switch (Child->Style->Align.y) 
                        {
                            case AlignTypeY::TOP: {
                                // TODO
                                break;
                            }
                            case AlignTypeY::MIDDLE: {
                                Child->Style->Position.y = (Style->Currentheight - Child->Style->Currentheight) / 2;
                                break;
                            }
                            case AlignTypeY::BOTTOM: {
                                // TODO
                                break;
                            }
                            default:
                                break;
                        }

                        switch (Child->Style->Align.x)
                        {
                            case AlignTypeX::LEFT: {
                                if(XIterationLeft + Child->Style->Currentwidth + Child->Style->Margin.Left + Child->Style->Margin.Right > Style->Currentwidth) {
                                    XIterationLeft = 0;
                                    YIterationLeft += MaxChildHeightLeft;
                                    MaxChildHeightLeft = 0;
                                    Child->Style->Position.x = XIterationLeft;
                                    Child->Style->Position.y = YIterationLeft;
                                } else {
                                    Child->Style->Position.x = XIterationLeft;
                                    Child->Style->Position.y = YIterationLeft;
                                    XIterationLeft += Child->Style->Currentwidth + Child->Style->Margin.Left + Child->Style->Margin.Right;
                                }

                                if(MaxChildHeightLeft < Child->Style->Currentheight + Child->Style->Margin.Top + Child->Style->Margin.Bottom){
                                    MaxChildHeightLeft = Child->Style->Currentheight + Child->Style->Margin.Top + Child->Style->Margin.Bottom;
                                }
                                break;
                            }
                            case AlignTypeX::CENTER: {
                                Child->Style->Position.x = (Style->Currentwidth - Child->Style->Currentwidth) / 2;
                                break;
                            }
                            case AlignTypeX::RIGHT: {
                                if(XIterationRight - Child->Style->Currentwidth - Child->Style->Margin.Left - Child->Style->Margin.Right < 0) {
                                    XIterationRight = Style->Currentwidth;
                                    YIterationRight += MaxChildHeightRight;
                                    MaxChildHeightRight = 0;
                                    Child->Style->Position.x = XIterationRight;
                                    Child->Style->Position.y = YIterationRight;
                                } else {
                                    XIterationRight -= Child->Style->Currentwidth + Child->Style->Margin.Right;
                                    Child->Style->Position.x = XIterationRight;
                                    Child->Style->Position.y = YIterationRight;
                                    XIterationRight -= Child->Style->Margin.Left;
                                }

                                if(MaxChildHeightRight < Child->Style->Currentheight + Child->Style->Margin.Top + Child->Style->Margin.Bottom) {
                                    MaxChildHeightRight = Child->Style->Currentheight + Child->Style->Margin.Top + Child->Style->Margin.Bottom;
                                }
                                break;
                            }
                            default:
                                break;
                        }
                    }

                    Child->UpdateFunction(Child);
                }
            }
        }
        IsRedraw = false;
    }

    void Component::Free() {
        this->Style->IsHidden = true;
        // TODO free
    }

    void Component::ClearChilds() {
        if(this->Childs) {
            for(uint64_t i = 0; i < this->Childs->length; i++) {
                Component* Child = (Component*)kot_vector_get(this->Childs, i);
                Child->Free();
            }
        }
    }

    void Component::AddChild(Component* Child) {
        // TODO sort by position first and by z-index

        Child->Deep = this->Deep + 1;

        if(!this->Childs){
            this->Childs = kot_vector_create();
        }

        Child->HorizontalOverflow = HorizontalOverflow;
        Child->VerticalOverflow = VerticalOverflow;
        
        Child->Parent = this;
        this->Index = kot_vector_push(this->Childs, Child);
    } 
}