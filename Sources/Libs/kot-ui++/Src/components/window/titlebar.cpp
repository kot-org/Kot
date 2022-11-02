#include <kot-ui++/component.h>

namespace Ui {

    Component* titlebar(char* title, TitlebarStyle style) {
        /* todo: drag region */
        Component* titlebar = UiLayout::Flexbox({ .width = 500, .height = 40, .backgroundColor = style.backgroundColor, .foregroundColor = style.foregroundColor }, Layout::HORIZONTAL);

        auto box = Ui::box({ .width = 30, .height = 20, .color = 0xFF0000 });
        titlebar->addChild(box);
        auto box2 = Ui::box({ .width = 30, .height = 30, .color = 0xFF00FF });
        titlebar->addChild(box2);

        return titlebar;
    }

}