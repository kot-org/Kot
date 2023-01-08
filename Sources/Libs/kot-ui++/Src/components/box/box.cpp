#include <kot-ui++/component.h>

namespace Ui {

    Component* box(BoxStyle style) {
        Component* box = new Component({ .Width = style.Width, .Height = style.Height, .backgroundColor = style.color });
        return box;
    }

}