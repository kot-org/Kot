#include <kot-ui++/component.h>

using namespace Ui;

namespace UiLayout {

    Component* Flexbox(Component::ComponentStyle Style) {
        Style.Display = Layout::FLEX;
        Component* flexbox = new Component(Style);

        return flexbox;
    }

}