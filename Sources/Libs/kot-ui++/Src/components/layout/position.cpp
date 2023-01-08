#include "layout.h"

using namespace Ui;

namespace UiLayout {

    void calculatePosition(Component* parent, uint32_t index) {

        Component* child = (Component*) vector_get(parent->getChilds(), index);
        Component* beforeChild = (Component*) vector_get(parent->getChilds(), index-1);

        switch(parent->getStyle()->position)
        {
            case Layout::RELATIVE:
            {
                child->getStyle()->x = beforeChild->getStyle()->x + beforeChild->getStyle()->Width;

                // todo: jump to the next line

                break;
            }

            case Layout::ABSOLUTE:
                break;
        }

    }  

}