#include "layout.h"

using namespace Ui;

namespace UiLayout {

    void calculateFlex(Component* parent, uint32_t index) {

        Component* child = (Component*) vector_get(parent->getChilds(), index);

        // todo: fill/normal mode
        
        switch (parent->getStyle()->direction)
        {
            case Layout::HORIZONTAL:
            {
                if(parent->getStyle()->height < child->getStyle()->height)
                    parent->getStyle()->height = child->getStyle()->height;

                break;
            }

            case Layout::VERTICAL:
            {
                

                break;
            }
        }

    }

}