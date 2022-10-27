#include <kot-ui++/component.h>

namespace Ui {

    Component* box(BoxStyle style) {
        Component* box = new Component({ .width = style.width, .height = style.height, .backgroundColor = style.color });
        return box;
    }

}