#include "layout.h"

using namespace Ui;

namespace UiLayout {

    void UiLayout::CalculateLayout(Component* parent){
        if(parent->GetChilds() != NULL){
            for(uint64_t i = 0; i < parent->GetChilds()->length; i++) {
                CalculateAlignment(parent, i);
                switch(parent->GetStyle()->Display)
                {
                    case Layout::FLEX:
                        CalculateFlex(parent, i);
                        break;

                    case Layout::GRID:
                        CalculateGrid(parent, i);
                        break;
                    
                    default:
                        break;
                }
                
                // here we check if the child isnt the first because he must have a brother
                if(i != 0)
                    CalculatePosition(parent, i);
            }

        }
    }

}