#include <kot-ui++/component.h>

using namespace Ui;

namespace UiLayout {

    Component* Flexbox(Component::ComponentStyle style, Layout::FlexDirection direction) {
        style.layout = Layout::FLEX;
        Component* flexbox = new Component(style);

        return flexbox;
    }

}