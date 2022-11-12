#include "layout.h"

namespace UiLayout {

    void calculateAlignment(Component* parent, uint32_t index) {

        Component* child = (Component*) vector_get(parent->getChilds(), index);

        /* Horizontal */
        switch(parent->getStyle()->horizontalAlign)
        {
            case Layout::LEFT:
                child->getStyle()->x = 0;
                break;

            case Layout::CENTER:
                child->getStyle()->x = parent->getStyle()->width / 2 - parent->getTotalWidthChilds() / 2;
                break;

            case Layout::RIGHT:
                child->getStyle()->x = parent->getStyle()->width - parent->getTotalWidthChilds();
                break;
        }

        /* Vertical */
        switch(parent->getStyle()->verticalAlign)
        {
            case Layout::TOP:
                child->getStyle()->y = 0;
                break;

            case Layout::MIDDLE:
                // todo: child->height   -->   getTotalHeightChilds
                child->getStyle()->y = parent->getStyle()->height / 2 - child->getStyle()->height / 2;
                break;

            case Layout::BOTTOM:
                // todo: child->height   -->   getTotalHeightChilds
                child->getStyle()->y = parent->getStyle()->height - child->getStyle()->height;
                break;
        }

    }

}