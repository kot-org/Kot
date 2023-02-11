#include "layout.h"

namespace UiLayout {

    void calculateAlignment(Component* parent, uint32_t index) {

        Component* child = (Component*) vector_get(parent->GetChilds(), index);

        /* Horizontal */
        switch(parent->GetStyle()->Align.X)
        {
            case Layout::LEFT:
                child->GetStyle()->X = 0;
                break;

            case Layout::CENTER:
                child->GetStyle()->X = parent->GetStyle()->Width / 2 - parent->GetTotalWidthChilds() / 2;
                break;

            case Layout::RIGHT:
                child->GetStyle()->X = parent->GetStyle()->Width - parent->GetTotalWidthChilds();
                break;
        }

        /* Vertical */
        switch(parent->GetStyle()->Align.Y)
        {
            case Layout::TOP:
                child->GetStyle()->Y = 0;
                break;

            case Layout::MIDDLE:
                // todo: child->Height   -->   GetTotalHeightChilds
                child->GetStyle()->Y = parent->GetStyle()->Height / 2 - child->GetStyle()->Height / 2;
                break;

            case Layout::BOTTOM:
                // todo: child->Height   -->   GetTotalHeightChilds
                child->GetStyle()->Y = parent->GetStyle()->Height - child->GetStyle()->Height;
                break;
        }

    }

}