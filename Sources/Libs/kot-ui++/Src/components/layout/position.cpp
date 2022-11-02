#include <kot-ui++/component.h>

using namespace Ui;

namespace UiLayout {

    void calculatePosition(Component* parent, Component* child) {
        uint32_t childIndex = parent->getChilds()->length;

        // calculate relative position
        if(childIndex != NULL && child->getStyle()->position == Layout::RELATIVE) {
            Component* cpntBeforeChild = (Component*) vector_get(parent->getChilds(), childIndex-1);
            Component* firstChild = (Component*) vector_get(parent->getChilds(), 0);

            if(cpntBeforeChild->getStyle()->x + cpntBeforeChild->getStyle()->width + child->getStyle()->width > parent->getStyle()->width) {
                child->getStyle()->x = 0;
                child->getStyle()->y = firstChild->getStyle()->height;
            } else {
                child->getStyle()->x += cpntBeforeChild->getStyle()->x + cpntBeforeChild->getStyle()->width;
            }
        }
    }  

}