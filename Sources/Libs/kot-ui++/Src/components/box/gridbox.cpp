#include <kot-ui++/component.h>

using namespace Ui;

namespace UiLayout {

    Component* Gridbox(Component::ComponentStyle Style) {
        Style.display = Layout::GRID;
        Component* gridbox = new Component(Style);

        return gridbox;
    }

}