#include <kot-ui++/component.h>

namespace Ui {

    Component* titlebar(char* title, TitlebarStyle style) {
        /* todo: drag region */
        Component* titlebar = UiLayout::Flexbox({ .direction = Layout::HORIZONTAL, .width = 500, .height = 40, .backgroundColor = style.backgroundColor, .foregroundColor = style.foregroundColor });

        auto box = Ui::box({ .width = 40, .height = 20, .color = 0xFF0000 });

        auto boxbox = Ui::box({ .width = 30, .height = 10, .color = 0xFF00FF });
        box->addChild(boxbox);
        
        titlebar->addChild(box);

        auto box2 = Ui::box({ .width = 40, .height = 30, .color = 0x00FF00 });
        titlebar->addChild(box2);

        auto box3 = Ui::box({ .width = 40, .height = 25, .color = 0x0000FF });
        titlebar->addChild(box3);

        return titlebar;
    }

}