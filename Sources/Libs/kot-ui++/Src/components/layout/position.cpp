#include "layout.h"

using namespace Ui;

namespace UiLayout {

    void calculatePosition(Component* parent, uint32_t index) {

        Component* child = (Component*) vector_get(parent->GetChilds(), index);
        Component* beforeChild = (Component*) vector_get(parent->GetChilds(), index-1);

        switch(parent->GetStyle()->position)
        {
            case Layout::RELATIVE:
            {
                child->GetStyle()->x = beforeChild->GetStyle()->x + beforeChild->GetStyle()->Width;

                // todo: jump to the next line

                break;
            }

            case Layout::ABSOLUTE:
                break;
        }

    }  

}