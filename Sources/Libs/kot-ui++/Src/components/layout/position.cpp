#include "layout.h"

using namespace Ui;

namespace UiLayout {

    void calculatePosition(Component* parent, uint32_t index) {

        Component* child = (Component*) vector_get(parent->GetChilds(), index);
        Component* beforeChild = (Component*) vector_get(parent->GetChilds(), index-1);

        switch(parent->GetStyle()->Position)
        {
            case Layout::RELATIVE:
            {
                child->GetStyle()->X = beforeChild->GetStyle()->X + beforeChild->GetStyle()->Width;

                // todo: jump to the next line

                break;
            }

            case Layout::ABSOLUTE:
                break;
        }

    }  

}