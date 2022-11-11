#include <kot-ui++/component.h>

namespace Ui {

    Component::Component(ComponentStyle style) {
        /* style */
        ComponentStyle* cpntStyle = (ComponentStyle*) malloc(sizeof(ComponentStyle));

        // layout
        cpntStyle->position = style.position;
        cpntStyle->display = style.display;
        cpntStyle->direction = style.direction;

        cpntStyle->width = style.width;
        cpntStyle->height = style.height;
        cpntStyle->fontSize = style.fontSize;
        cpntStyle->borderRadius = style.borderRadius;

        cpntStyle->backgroundColor = style.backgroundColor;
        cpntStyle->foregroundColor = style.foregroundColor;

        cpntStyle->x = style.x; 
        cpntStyle->y = style.y;

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

        cpntStyle->backgroundColor = 0x181818;

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

    void Component::setWidth(uint32_t width) {
        this->style->width = width;
    }
    
    void Component::setHeight(uint32_t height) {
        this->style->height = height;
    }

    Component::ComponentStyle* Component::getStyle() {
        return this->style;
    }

    vector_t* Component::getChilds() {
        return this->childs;
    }

    Component* Component::getParent() {
        return this->parent;
    }

    void Component::update() {
        UiLayout::calculateLayout(this);
        
        this->draw();
        
        if(childs != NULL) {
            for(int i = 0; i < childs->length; i++) {
                Component* child = (Component*) vector_get(childs, i);

                if(child->fb == NULL)
                    child->draw();
                
                blitFramebuffer(this->fb, child->fb, child->style->x, child->style->y);
            }
        }

        if(parent != NULL)
            parent->update();
    }

    void Component::draw() {
        if(this->fb == NULL)
            this->createFramebuffer(this->style->width, this->style->height);

        fillRect(this->fb, 0, 0, this->style->width, this->style->height, this->style->backgroundColor);
    }

    void Component::addChild(Component* child) {
        if(!this->childs)
            this->childs = vector_create();
        
        child->parent = this;
        vector_push(this->childs, child);

        if(!child->childs)
            vector_push(lastComponents, child);
    }  

}