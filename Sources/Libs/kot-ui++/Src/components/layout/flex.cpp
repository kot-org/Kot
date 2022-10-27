#include "layout.h"

namespace Ui {

    FlexLayout::FlexLayout(FlexDirection direction) {
        Component* flexCpnt = new Component({});

        this->cpnt = flexCpnt;
    }

}