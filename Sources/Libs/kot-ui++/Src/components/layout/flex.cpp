#include "layout.h"

namespace Ui {

    FlexLayout::FlexLayout(FlexDirection direction) {
        Component* flexCpnt = new Component({ .width = 500, .height = 40 }); // width 500 is a test (replace by 100%)

        this->cpnt = flexCpnt;
    }

}