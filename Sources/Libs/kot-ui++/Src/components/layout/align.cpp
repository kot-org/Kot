#include "layout.h"

namespace UiLayout {

    void calculateAlignment(Component* parent, uint32_t index) {

        Component* child = (Component*) vector_get(parent->GetChilds(), index);

        /* Horizontal */
        switch(parent->GetStyle()->Align.x)
        {
            case Layout::LEFT:
                child->GetStyle()->x = 0;
                break;

            case Layout::CENTER:
                child->GetStyle()->x = parent->GetStyle()->Width / 2 - parent->GetTotalWidthChilds() / 2;
                break;

            case Layout::RIGHT:
                child->GetStyle()->x = parent->GetStyle()->Width - parent->GetTotalWidthChilds();
                break;
        }

        /* Vertical */
        switch(parent->GetStyle()->Align.y)
        {
            case Layout::TOP:
                child->GetStyle()->y = 0;
                break;

            case Layout::MIDDLE:
                // todo: child->Height   -->   GetTotalHeightChilds
                child->GetStyle()->y = parent->GetStyle()->Height / 2 - child->GetStyle()->Height / 2;
                break;

            case Layout::BOTTOM:
                // todo: child->Height   -->   GetTotalHeightChilds
                child->GetStyle()->y = parent->GetStyle()->Height - child->GetStyle()->Height;
                break;
        }

    }

}