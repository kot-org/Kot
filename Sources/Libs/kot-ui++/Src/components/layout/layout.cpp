#include "layout.h"

using namespace Ui;

namespace UiLayout {

    void UiLayout::calculateLayout(Component* parent) {
        if(parent->getChilds() != NULL) {

            for(int i = 0; i < parent->getChilds()->length; i++) {

                calculateAlignment(parent, i);

                if(parent->getStyle()->display == Layout::FLEX)
                    calculateFlex(parent, i);
                
                // here we check if the child isnt the first because he must have a brother
                if(i != 0)
                    calculatePosition(parent, i);
            }

        }
    }

}