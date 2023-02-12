#include "layout.h"

using namespace Ui;

namespace UiLayout {

    void CalculateFlex(Component* parent, uint32_t index) {
        Component* child = (Component*) vector_get(parent->GetChilds(), index);

        // todo: fill/normal mode
        
        switch (parent->GetStyle()->Direction)
        {
            case Layout::HORIZONTAL:
            {      
                // todo: parent -> Width: 100%

                if(parent->GetStyle()->Height < child->GetStyle()->Height)
                    parent->GetStyle()->Height = child->GetStyle()->Height;

                /* Space */
                if(parent->GetStyle()->Space == Layout::BETWEEN) {
                    
                    

                } // else around...
                parent->UpdateFramebuffer(parent->Parent->GetStyle()->Width, parent->GetStyle()->Height);
                break;
            }

            case Layout::VERTICAL:
            {
                

                break;
            }
        }

        // if the child is the last
        if(index+1 == parent->GetChilds()->length)
            RecalculateFlexChilds(parent);

    }

    void RecalculateFlexChilds(Component* parent) {

        for(int i = 0; i < parent->GetChilds()->length; i++) {

            Component* child = (Component*) vector_get(parent->GetChilds(), i);

            child->GetStyle()->Height = parent->GetStyle()->Height;

        }

    }

}