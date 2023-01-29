#include <kot-ui++/component.h>

namespace Ui {

    Component::Component(ComponentStyle style) {
        /* style */
        ComponentStyle* cpntStyle = (ComponentStyle*) malloc(sizeof(ComponentStyle));

        // layout
        cpntStyle->position = style.position;
        cpntStyle->display = style.display;
        cpntStyle->direction = style.direction;
        cpntStyle->align = style.align;
        cpntStyle->space = style.space;

        cpntStyle->Width = style.Width;
        cpntStyle->Height = style.Height;
        cpntStyle->fontSize = style.fontSize;
        cpntStyle->borderRadius = style.borderRadius;

        cpntStyle->backgroundColor = style.backgroundColor;
        cpntStyle->foregroundColor = style.foregroundColor;

        cpntStyle->x = style.x; 
        cpntStyle->y = style.y;

        createFramebuffer(cpntStyle->Width, cpntStyle->Height);

        /* component */
        this->style = cpntStyle;
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

        /* style */
        ComponentStyle* cpntStyle = (ComponentStyle*) malloc(sizeof(ComponentStyle));

        cpntStyle->Width = fb->Width;
        cpntStyle->Height = fb->Height;

        /* component */
        this->fb = cpntFb;
        this->style = cpntStyle;
    }

    /* Component Framebuffer */
    void Component::createFramebuffer(uint32_t Width, uint32_t Height) {
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

        this->fb = cpntFb;
    }
    
    void Component::updateFramebuffer(uint32_t Width, uint32_t Height) {

        if(fb->Width != Width || fb->Height != Height) {
            uint32_t Pitch = Width * 4;

            fb->Size = Height * Pitch;
            fb->Buffer = calloc(fb->Size);
            fb->Pitch = Pitch;
            fb->Width = Width;
            fb->Height = Height;
        }

    }
    
    framebuffer_t* Component::GetFramebuffer() {
        return this->fb;
    }

    Component::ComponentStyle* Component::getStyle() {
        return this->style;
    }

    vector_t* Component::getChilds() {
        return this->childs;
    }

    uint32_t Component::getTotalWidthChilds() {
        return this->totalWidthChilds;
    }

    uint32_t Component::getTotalHeightChilds() {
        return this->totalHeightChilds;
    }

/*     void Component::update() {
        UiLayout::calculateLayout(this);
        
        this->draw();
        
        if(childs != NULL) {

            for(int i = 0; i < childs->length; i++) {
                Component* child = (Component*) vector_get(childs, i);

                if(child->fb == NULL)
                    child->draw();
                
                BlitFramebuffer(this->fb, child->fb, child->style->x, child->style->y);
                child->IsBlit = true;
            }

        }

        if(parent != NULL)
            parent->update();
    } */

    void Component::update() {
        UiLayout::calculateLayout(this);

        this->draw();

        if(childs != NULL) {

            for(int i = 0; i < childs->length; i++) {
                Component* child = (Component*) vector_get(childs, i);

                if(child->ReadyToBlit == true)
                    BlitFramebuffer(this->fb, child->fb, child->style->x, child->style->y);
                else 
                    child->update();

            }

        }
        this->ReadyToBlit = true;

        if(parent != NULL)
            parent->update();
    }

    void Component::draw() {
        updateFramebuffer(this->style->Width, this->style->Height);
        //Printlog("draw");

        FillRect(this->fb, 0, 0, this->style->Width, this->style->Height, this->style->backgroundColor);
    }

    void Component::addChild(Component* child) {
        if(!this->childs)
            this->childs = vector_create();
        
        child->parent = this;
        vector_push(this->childs, child);

        this->totalWidthChilds += child->style->Width;
        // todo: if there is a new line, add the Height to totalHeightChilds (this->totalHeightChilds += child->style->Height;)
    }  

}