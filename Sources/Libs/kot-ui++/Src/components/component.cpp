#include <kot-ui++/component.h>

namespace Ui {

    Component::Component(ComponentStyle style) {
        /* framebuffer */
        framebuffer_t* cpntFb = (framebuffer_t*) malloc(sizeof(framebuffer_t));

        uint32_t bpp = 32, btpp = 4;

        uint32_t pitch = style.width * btpp;

        cpntFb->size = style.height * pitch;
        cpntFb->addr = calloc(cpntFb->size);
        cpntFb->pitch = pitch;
        cpntFb->width = style.width;
        cpntFb->height = style.height;
        cpntFb->bpp = bpp;
        cpntFb->btpp = btpp;

        /* style */
        ComponentStyle* cpntStyle = (ComponentStyle*) malloc(sizeof(ComponentStyle));

        cpntStyle->position = style.position;
        cpntStyle->layout = style.layout;

        cpntStyle->width = style.width;
        cpntStyle->height = style.height;
        cpntStyle->fontSize = style.fontSize;
        cpntStyle->borderRadius = style.borderRadius;

        cpntStyle->backgroundColor = style.backgroundColor;
        cpntStyle->foregroundColor = style.foregroundColor;

        cpntStyle->x = style.x; 
        cpntStyle->y = style.y;

        /* component */
        this->fb = cpntFb;
        this->style = cpntStyle;
    }

    Component::Component(framebuffer_t* fb, ComponentStyle style) {
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

        cpntStyle->position = style.position;
        cpntStyle->layout = style.layout;

        cpntStyle->width = style.width;
        cpntStyle->height = style.height;
        cpntStyle->fontSize = style.fontSize;
        cpntStyle->borderRadius = style.borderRadius;

        cpntStyle->backgroundColor = style.backgroundColor;
        cpntStyle->foregroundColor = style.foregroundColor;

        cpntStyle->x = style.x; 
        cpntStyle->y = style.y;

        /* component */
        this->fb = cpntFb;
        this->style = cpntStyle;
    }

    Component::ComponentStyle* Component::getStyle() {
        return this->style;
    }

    vector_t* Component::getChilds() {
        return this->childs;
    }

    void Component::draw() {
        Component* cpnt = this;

        fillRect(cpnt->fb, 0, 0, cpnt->style->width, cpnt->style->height, cpnt->style->backgroundColor);

        if(cpnt->childs) {
            for(int i = cpnt->childs->length - 1; i >= 0; i--) {
                Component* child = (Component*) vector_get(cpnt->childs, i);
                
                blitFramebuffer(cpnt->fb, child->fb, child->style->x, child->style->y);
            }
        }
    }

    void Component::addChild(Component* child) {
        if(!this->childs)
            this->childs = vector_create();

        /* calculate */

        UiLayout::calculatePosition(this, child);

        // here i put the childs vector and the child to push the child into the vector after calculating
        if(this->style->layout == Layout::FLEX)
            UiLayout::calculateFlex(this, this->childs, child); 

        child->parent = this;
        vector_push(this->childs, child); 


        child->draw();
    }  

}