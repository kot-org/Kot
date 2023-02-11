#include <kot-ui++/component.h>

namespace Ui {

    Component* Box(BoxStyle Style) {
        Component* box = new Component({ .Width = Style.Width, .Height = Style.Height, .backgroundColor = Style.color });
        return box;
    }

}