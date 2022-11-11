#include "layout.h"

using namespace Ui;

namespace UiLayout {

    void UiLayout::calculateLayout(Component* parent) {
        if(parent->getChilds() != NULL) {

            for(int i = 0; i < parent->getChilds()->length; i++) {

                if(i != 0)
                    calculatePosition(parent, i);

                // todo: flex
                
            }

        }
    }

}