#include <kot-ui++/component.h>

using namespace Ui;

namespace UiLayout {

    Component* Gridbox(Component::ComponentStyle style) {
        style.display = Layout::GRID;
        Component* gridbox = new Component(style);

        return gridbox;
    }

}