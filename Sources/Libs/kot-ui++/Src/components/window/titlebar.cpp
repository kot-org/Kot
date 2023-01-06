#include <kot-ui++/component.h>

namespace Ui {

    Component* titlebar(char* title, TitlebarStyle style) {
        /* todo: drag region */
        /* todo: replace width: 500 -> 100% */
        Component* titlebar = UiLayout::Flexbox({ .direction = Layout::HORIZONTAL, .space = Layout::BETWEEN, .width = 500, .backgroundColor = style.backgroundColor, .foregroundColor = style.foregroundColor });

        auto windowInfo = Ui::box({ .width = titlebar->getStyle()->width/2, .height = 40, .color = 0xFF0000 });

        auto iconBox = Ui::box({ .width = 20, .height = 20, .color = 0xFFFF00 });
        windowInfo->addChild(iconBox); 
        auto titleBox = Ui::box({ .width = 40, .height = 25, .color = 0xFF00FF });
        windowInfo->addChild(titleBox); 
        
        titlebar->addChild(windowInfo);

        auto buttons = Ui::box({ .width = 60, .color = 0x00FF00 });

        /* auto box2box = Ui::box({ .width = 10, .height = 10, .color = 0xFFFF00 });
        buttons->addChild(box2box);  */

        titlebar->addChild(buttons);

        return titlebar;
    }

}