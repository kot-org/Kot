#include <kot-ui++/component.h>
#include <kot-ui++/components/layout/layout.h>

namespace Ui {

    Component* titlebar(char* title, TitlebarStyle style) {
        /* todo: drag region */
        /* todo: remplacer .width = 500 par .width = 100% */
        Component* titlebar = new Component({ .width = 500, .height = 40, .backgroundColor = style.backgroundColor, .foregroundColor = style.foregroundColor });

        auto flexbox = (new Ui::FlexLayout(Ui::FlexLayout::HORIZONTAL))->cpnt;

        auto box = Ui::box({ .width = 30, .height = 20, .color = 0xFF0000 });
        flexbox->addChild(box);
        auto box2 = Ui::box({ .width = 30, .height = 20, .color = 0xFF00FF });
        flexbox->addChild(box2);

        titlebar->addChild(flexbox);

        return titlebar;
    }

}