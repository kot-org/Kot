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

        cpntStyle->width = style.width;
        cpntStyle->height = style.height;
        cpntStyle->fontSize = style.fontSize;
        cpntStyle->borderRadius = style.borderRadius;

        cpntStyle->backgroundColor = style.backgroundColor;
        cpntStyle->foregroundColor = style.foregroundColor;

        cpntStyle->x = style.x; 
        cpntStyle->y = style.y;

        createFramebuffer(cpntStyle->width, cpntStyle->height);

        /* component */
        this->style = cpntStyle;
    }

    Component::Component(framebuffer_t* fb) {
        /* framebuffer */
        framebuffer_t* cpntFb = (framebuffer_t*) malloc(sizeof(framebuffer_t));

        cpntFb->size = fb->size;
        cpntFb->addr = fb->addr;
        cpntFb->pitch = fb->pitch;
        cpntFb->width = fb->width;
        cpntFb->height = fb->height;
        cpntFb->bpp = fb->bpp;
        cpntFb->btpp = fb->btpp;

        /* style */
        ComponentStyle* cpntStyle = (ComponentStyle*) malloc(sizeof(ComponentStyle));

        cpntStyle->width = fb->width;
        cpntStyle->height = fb->height;

        /* component */
        this->fb = cpntFb;
        this->style = cpntStyle;
    }

    /* Component Framebuffer */
    void Component::createFramebuffer(uint32_t width, uint32_t height) {
        framebuffer_t* cpntFb = (framebuffer_t*) malloc(sizeof(framebuffer_t));

        uint32_t bpp = 32, btpp = 4;

        uint32_t pitch = width * btpp;

        cpntFb->size = height * pitch;
        cpntFb->addr = calloc(cpntFb->size);
        cpntFb->pitch = pitch;
        cpntFb->width = width;
        cpntFb->height = height;
        cpntFb->bpp = bpp;
        cpntFb->btpp = btpp;

        this->fb = cpntFb;
    }
    
    void Component::updateFramebuffer(uint32_t width, uint32_t height) {

        if(fb->width != width || fb->height != height) {
            uint32_t pitch = width * 4;

            fb->size = height * pitch;
            fb->addr = calloc(fb->size);
            fb->pitch = pitch;
            fb->width = width;
            fb->height = height;
        }

    }
    
    framebuffer_t* Component::getFramebuffer() {
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
                
                blitFramebuffer(this->fb, child->fb, child->style->x, child->style->y);
                child->IsBlit = true;
            }

        }

        if(parent != NULL)
            parent->update();
    } */

    void Component::update() {
        UiLayout::calculateLayout(this);
        
        //this->draw();

        if(parent != NULL){
            blitFramebuffer(parent->fb, this->fb, this->style->x, this->style->y);
            parent->update();
        }

    }

    void Component::draw() {
        updateFramebuffer(this->style->width, this->style->height);
        Printlog("draw");

        fillRect(this->fb, 0, 0, this->style->width, this->style->height, this->style->backgroundColor);
    }

    void Component::addChild(Component* child) {
        if(!this->childs)
            this->childs = vector_create();
        
        child->parent = this;
        vector_push(this->childs, child);

        this->totalWidthChilds += child->style->width;
        // todo: if there is a new line, add the height to totalHeightChilds (this->totalHeightChilds += child->style->height;)

        /* if(!child->childs && this->IsChildIsInLastComponent == false) {
            vector_push(lastComponents, child);
            this->IsChildIsInLastComponent = true;
        } */
    }  

}