#include "layout.h"

namespace UiLayout {

    void calculateAlignment(Component* parent, uint32_t index) {

        Component* child = (Component*) vector_get(parent->getChilds(), index);

        /* Horizontal */
        switch(parent->getStyle()->align.x)
        {
            case Layout::LEFT:
                child->getStyle()->x = 0;
                break;

            case Layout::CENTER:
                child->getStyle()->x = parent->getStyle()->Width / 2 - parent->getTotalWidthChilds() / 2;
                break;

            case Layout::RIGHT:
                child->getStyle()->x = parent->getStyle()->Width - parent->getTotalWidthChilds();
                break;
        }

        /* Vertical */
        switch(parent->getStyle()->align.y)
        {
            case Layout::TOP:
                child->getStyle()->y = 0;
                break;

            case Layout::MIDDLE:
                // todo: child->Height   -->   getTotalHeightChilds
                child->getStyle()->y = parent->getStyle()->Height / 2 - child->getStyle()->Height / 2;
                break;

            case Layout::BOTTOM:
                // todo: child->Height   -->   getTotalHeightChilds
                child->getStyle()->y = parent->getStyle()->Height - child->getStyle()->Height;
                break;
        }

    }

}