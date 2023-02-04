#include "layout.h"

using namespace Ui;

namespace UiLayout {

    void UiLayout::calculateLayout(Component* parent) {
        if(parent->GetChilds() != NULL) {

            for(int i = 0; i < parent->GetChilds()->length; i++) {

                calculateAlignment(parent, i);

                switch(parent->GetStyle()->display)
                {
                    case Layout::FLEX:
                        calculateFlex(parent, i);
                        break;

                    case Layout::GRID:
                        calculateGrid(parent, i);
                        break;
                    
                    default:
                        break;
                }
                
                // here we check if the child isnt the first because he must have a brother
                if(i != 0)
                    calculatePosition(parent, i);
            }

        }
    }

}